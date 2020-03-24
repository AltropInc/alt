#pragma once

#include "Mutex.h"
#include <mutex>                // For mutex
#include <atomic>               // For atomic
#include  <condition_variable>  // For condition_variable

namespace alt{

#if (0)
struct CVNone
{
  public:
    void notify_one() noexcept {}
    void notify_all() noexcept {}
    void wait(std::unique_lock<std::mutex>&) {}
    template<class Predicate>
    void wait( std::unique_lock<std::mutex>& lock, Predicate pred ) {}
    template< class Rep, class Period >
    std::cv_status wait_for( std::unique_lock<std::mutex>,
                         const std::chrono::duration<Rep, Period>)
                         { return std::cv_status::timeout; }
    template< class Rep, class Period, class Predicate >
    bool wait_for( std::unique_lock<std::mutex>&,
                const std::chrono::duration<Rep, Period>&,
                Predicate) { return true; }
    template< class Clock, class Duration >
    std::cv_status wait_until( std::unique_lock<std::mutex>&,
                const std::chrono::time_point<Clock, Duration>&)
                        { return std::cv_status::timeout; }
                        template< class Clock, class Duration, class Pred >
    bool wait_until( std::unique_lock<std::mutex>&,
                 const std::chrono::time_point<Clock, Duration>&,
                 Pred ) { return true; }
};
#endif

template <typename Mutex, typename CV>
class SemaphoreT 
{
  public:
    using native_handle_type = typename CV::native_handle_type;
    explicit SemaphoreT(size_t init_count=0): count_ {init_count} {}

    ALT_INLINE void notify()
    {
        std::lock_guard<Mutex> lock {mutex_};
        ++count_;
        cv_.notify_one();
    }

    ALT_INLINE void wait()
    {
        std::unique_lock<Mutex> lock{mutex_};
        cv_.wait(lock, [&]{ return count_ > 0; });
        --count_;        
    }

    ALT_INLINE bool try_wait()
    {
        std::lock_guard<Mutex> lock{mutex_};
        if (count_ > 0)
        {
            --count_;
            return true;
        }
        return false;        
    }

    template<class Rep, class Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& d)
    {
        std::unique_lock<Mutex> lock{mutex_};
        auto finished = cv_.wait_for(lock, d, [&]{ return count_ > 0; });
        if (finished)
            --count_;
        return finished;
    }

    template<class Clock, class Duration>
    bool wait_until(const std::chrono::time_point<Clock, Duration>& t)
    {
        std::unique_lock<Mutex> lock{mutex_};
        auto finished = cv_.wait_until(lock, t, [&]{ return count_ > 0; });
        if (finished)
            --count_;
        return count_;
    }

    native_handle_type native_handle()
    {
        return cv_.native_handle();
    }

  private:
    SemaphoreT(const SemaphoreT&) = delete;
    SemaphoreT(SemaphoreT&&) = delete;
    SemaphoreT& operator=(const SemaphoreT&) = delete;
    SemaphoreT& operator=(SemaphoreT&&) = delete;
    Mutex    mutex_;
    CV       cv_;
    size_t   count_;
};

using Semaphore = SemaphoreT<std::mutex, std::condition_variable>;

class SpinSemaphore
{
  public:
    SpinSemaphore (uint64_t init_count=0): count_ {init_count} {}

    ALT_INLINE void notify()
    {
        auto count = count_.load(std::memory_order_acquire);
        while (!count_.compare_exchange_weak(
                    count, count + 1,
                    std::memory_order_release, std::memory_order_acquire))
        {
            pause();
        }
    }

    ALT_INLINE bool try_wait()
    {
        auto count = count_.load(std::memory_order_acquire);
        if (count > 0)
        {
            return count_.compare_exchange_strong(count, count-1);
                        //std::memory_order_acq_rel, std::memory_order_relaxed);
                        //std::memory_order_release, std::memory_order_acquire);
        }
        return false;
    }

    ALT_INLINE void wait()
    {
        while(!try_wait())
        {
            pause();
        }
    }

  private:
    std::atomic<uint64_t> count_;
};

template <typename SemaphoreT>
class ScopedSemaphore
{
  public:
    explicit ScopedSemaphore(SemaphoreT& sem) : semaphore_(sem)
    { semaphore_.wait(); }
    ~ScopedSemaphore() { semaphore_.notify(); }
  private:
    ScopedSemaphore(const ScopedSemaphore&) = delete;
    ScopedSemaphore(ScopedSemaphore&&) = delete;
    ScopedSemaphore& operator=(const ScopedSemaphore&) = delete;
    ScopedSemaphore& operator=(ScopedSemaphore&&) = delete;
    SemaphoreT& semaphore_;
};

}