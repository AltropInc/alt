#include "Thread.h"                      // for Thread, this class

#include <util/system/SysError.h>        // for SYS_ERR_THROW

#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)
#include <windows.h>
#else
#include <sched.h>                       // for POSIX sched
#endif

namespace alt
{

Thread::Thread() : reactor_(this)
{
}

void Thread::setPriority(ThreadPriority priority)
{
	if (priority != priority_)
	{
		priority_= priority;
        if (thread_)
		{
#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)
            int win_prio;
            switch (priority_)
            {
            case ThreadPriority::Lowest:
                win_prio = THREAD_PRIORITY_LOWEST;
            case ThreadPriority::Low:
                win_prio = THREAD_PRIORITY_BELOW_NORMAL;
            case ThreadPriority::Normal:
                win_prio = THREAD_PRIORITY_NORMAL;
            case ThreadPriority::High:
                win_prio = THREAD_PRIORITY_ABOVE_NORMAL;
            case ThreadPriority::Highest:
                win_prio = THREAD_PRIORITY_HIGHEST;
            }
			if (SetThreadPriority(thread_->native_handle() ,win_prio) == 0)
            {
                SYS_ERR_THROW(ThreadException);
            }
#else
		    // policy = SCHED_OTHER;
            int posix_prio;
            int min_prio = sched_get_priority_min(SCHED_OTHER);
            int max_prio = sched_get_priority_max(SCHED_OTHER);
            switch (priority_)
            {
            case ThreadPriority::Lowest:
                posix_prio = min_prio;
            case ThreadPriority::Low:
                posix_prio = min_prio + (max_prio - min_prio) / 4;
            case ThreadPriority::Normal:
                posix_prio = min_prio + (max_prio - min_prio) / 2;
            case ThreadPriority::High:
                posix_prio = min_prio + 3 * (max_prio - min_prio) / 4;
            case ThreadPriority::Highest:
                posix_prio = max_prio;
            }
            struct sched_param par;
			par.sched_priority = posix_prio;
			if (pthread_setschedparam(thread_->native_handle(), SCHED_OTHER, &par))
            {
                SYS_ERR_THROW(ThreadException);
            }
#endif
		}
	}
}

void Thread::setAffinity(int cpu_index)
{
    if (!thread_)
    {
        return;
    }
#if (ALT_OS_FAMILY==ALT_OS_FMAILY_WINDOWS)
	DWORD mask = 1;
	mask <<= cpu_index;
    if (SetThreadAffinityMask(thread_->native_handle(), mask) == 0)
    {
        SYS_ERR_THROW(ThreadException);
    }
#elif (ALT_OS_FAMILY==ALT_OS_FAMILY_MAC)
	thread_affinity_policy policy;
	policy.affinity_tag = cpu_index;
	kern_return_t rc = thread_policy_set(pthread_mach_thread_np(thread_->native_handle()),
		THREAD_AFFINITY_POLICY,
		(thread_policy_t)&policy,
		THREAD_AFFINITY_POLICY_COUNT);
	if (rc != KERN_SUCCESS)
	{
		SYS_ERR_THROW(ThreadException);;
	}
#else
    cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu_index, &cpuset);
    int rc = pthread_setaffinity_np(thread_->native_handle(), sizeof(cpu_set_t), &cpuset);
 	if (rc != 0)
	{
		SYS_ERR_THROW(ThreadException);
	}
#endif
    cpu_bound_ = cpu_index;
}

int Thread::getAffinity() const
{
    if (!thread_)
    {
        // thread not created yet, so not bound either
        return -1;
    }

    int cpu_bound = -1;
#if (ALT_OS_FAMILY==ALT_OS_FMAILY_WINDOWS)
    cpu_bound = cpu_bound_;
#elif (ALT_OS_FAMILY==ALT_OS_FAMILY_MAC)
	thread_affinity_policy policy;
	mach_msg_type_number_t count = THREAD_AFFINITY_POLICY_COUNT;
	boolean_t get_default = false;
	kern_return_t rc = thread_policy_get(
        pthread_mach_thread_np(thread_->native_handle()),
		THREAD_AFFINITY_POLICY,
		(thread_policy_t)&policy,
		&count,
		&get_default);
    if (rc != KERN_SUCCESS)
    {
        SYS_ERR_THROW(ThreadException);
    }
	cpu_bound = policy.affinity_tag >= std::thread::hardware_concurrency()
        ? -1 :policy.affinity_tag;
#else
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	if (pthread_getaffinity_np(thread_->native_handle(), sizeof(cpuset), &cpuset) != 0)
    {
        SYS_ERR_THROW(ThreadException);
    }
	int cpu_num = std::thread::hardware_concurrency(); // pthread_num_processors_np();
	for (int i = 0; i < cpu_num; i++)
	{
		if (CPU_ISSET(i, &cpuset))
		{
			cpu_bound = i;
			break;
		}
	}
#endif
    return cpu_bound;
}

int Thread::getCPU() const
{
    return sched_getcpu();
}

void Thread::join()
{
    if (thread_ && thread_->joinable())
    {
        thread_->join();
    }
    thread_.reset(nullptr);
}

void Thread::setTerminated()
{
    thread_status_.store(e_Terminated);
    terminated_.set_value(true);
}

bool Thread::isTerminated()
{
    return thread_status_.load()>=e_Terminated;
}

void Thread::signalTermination()
{
    reactor_.stop();
}

void Thread::onReactorStop()
{
    setTerminated();
}

void Thread::terminate(Clock::tick_type grace_period)
{
    if (isTerminated())
    {
        return;
    }
    
    // signal thread to terminate
    reactor_.stop();

    // check thread terminated_ promise. The thread must check the reactor's 
    // terminate_signal_ regularly and call setTerminated to exit once the signal is set.
    std::future<bool> future_obj = terminated_.get_future();
    if (future_obj.wait_for(std::chrono::milliseconds(Clock::millisecs(grace_period)))
        ==std::future_status::timeout)
    {
        if (!isTerminated())
        {
            // Something wrong, the thread is not responsive to terminate signal.
            // Force to terminate.
            // TODO: this is OS dependent. Here we assume PThread is used (OSX and Linux).
            auto hd = thread_->native_handle();
            pthread_cancel(hd);
            thread_status_.store(e_Terminated);
        }
    }
    join();
}

} // namespace alt