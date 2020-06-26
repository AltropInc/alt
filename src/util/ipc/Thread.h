#pragma once

#include <util/system/Platform.h>
#include <util/Defs.h>              // for ALT_UTIL_PUBLIC
#include <util/types/Enum.h>        // for Enum
#include <util/system/Reactor.h>    // for Reactor

#include <thread>                   // for std::thread
#include <atomic>                   // for atomic
#include <future>                   // for promise
#include <memory>                   // for unique_ptr

namespace alt
{

ENUM (ThreadPriority, uint8_t, Lowest, Low, Normal, High, Highest);

/**
 * \class Thread
 * \ingroup IPC
 * \brief  Implements a thread wrapper to std::thread with a reactor. If a thread wants
 * to use the reactor, it must call reactor().run() to start the reactor, see Reactor. If a
 * thread wants to do its own loop without using the reactor, typically the thread
 * performs a task without the need to interact with outside events, the thread should
 * periodically check reactor().stopoed() even though the reactor is not started,
 * because this is the signal from outside to indicate the thread needs to stop its work
 * and exit.
 */
class ALT_UTIL_PUBLIC Thread : public ReactorOwner
{
public:
    enum Status
    {
        e_Inactive,
        e_Started,
        e_Terminated,
    };

    enum ExecutionResult
    {
        e_Succeeded,   ///< terminated with success
        e_Unfinished,  ///< unfinished, not executed
        e_Failed,      ///< terminated with failure
        e_Killed,      ///< killed with unfinished work
        e_Aborted,     ///< aborted with unfinished work
    };

    Thread();

    template <class Fn, class... Args>
	bool start(Fn&& fn, Args&&... args)
    {
        if (thread_)
        {
            // cannot start twice
            return false;
        }
        thread_.reset(new std::thread(fn, std::forward(args)...));
        //AppContext::instance().registerThread(this);
        thread_status_.store(e_Started);
        return true;
    }
		
	bool isAlive() { return thread_.get() != nullptr; }

    /// \brief return underlying std thread
	std::thread* underlyingThread() { return thread_.get(); }

    /// \brief sets the thread's priority.
    void setPriority(ThreadPriority priority);

    /// \brief gets the thread's priority.
    ThreadPriority getPriority() const { return priority_; };

    /// \brief binds the thread to run on a particular cpu core. This must be called
    /// at the start the thread code, typically before entering the reactor's loop.
    /// If the call is successful, and the thread is not currently running on one of the
    /// given CPU, then the thread will be migrated to the given CPU. An exception will
    /// be raised if the call fails.
    void setAffinity(int cpu_index);

    /// \brief tells if the thread is bound to a cpu core, -1 if not bound
	int getAffinity() const;

    /// \brief tells which cpu this thread is currently running. This must be called from
    /// this thread code
	int getCPU() const;

    /// \brief set to terminated status, called from this thread to flag ternimated status
    void setTerminated();

    /// \brief wait for this thread to finish, called only from the parent thread
	void join();

    /// \brief check if this thread is terminated
    bool isTerminated();

    /// \brief signal termination called from outside of this thread. 
	void signalTermination();

    /// \brief signal termination and wait for this thread to finish, called only from
    /// the parent thread who is responsible to remove this thread object. If this thread
    /// does not finish within a graceful period, terminate it forcefully.
    /// \param grace_period graceful period
	void terminate(Clock::tick_type grace_period=Clock::one_sec);

    /// \brief callback interface of ReactorOwner, called when the reactor main loop exit
    virtual void onReactorStop() override;

    /// \brief return the reactor of this thread
    Reactor& reactor() { return reactor_; }

    /// \brief return true if this is the main thread
    virtual bool isMainThread() { return false; }

    Status status() { return thread_status_.load(); }

  protected:

	std::unique_ptr<std::thread>	 thread_;
    Reactor                          reactor_;
	std::atomic<Status>			     thread_status_ {e_Inactive};
    std::promise<bool>               terminated_;
    ThreadPriority                   priority_{ThreadPriority::Normal};
    int                              cpu_bound_{-1};
};


class MainThread : protected Thread
{
  public:

    bool isMainThread() override { return true; }

    MainThread();
};

}  // namespace alt
