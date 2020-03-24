#pragma once

/**
 * @file Mutex.h
 * @brief Defines classes used by threads and processes
 * @author David Shang
 * @version 1.00
 * $Id: $
 */

#include <util/sysinfo/Platform.h>
#include <mutex>            // For mutex
#include <atomic>           // For atomic

#if defined (__GNUC__)
#include <xmmintrin.h>      // For _mm_pause
#endif

namespace alt{

#if defined (__GNUC__)
ALT_INLINE void pause () { _mm_pause(); }
ALT_INLINE void barrier () { asm volatile("":::"memory"); }
ALT_INLINE void fence () {  __sync_synchronize(); }
#else
ALT_INLINE void pause () { }
ALT_INLINE void barrier () { }
ALT_INLINE void fence () { }
#endif

struct MutexNone
{
  public:
    void lock() {}
    void unlock() noexcept {}
    bool try_lock() { return true; }
};

class SpinMutex
{
    std::atomic_flag  flag_ {false};
  public:
    SpinMutex() {}
    ALT_INLINE void lock()
    {
        while (flag_.test_and_set(std::memory_order_acquire))
        {
            // Not really pause, just tell cpu that it is in spinning
            pause();
        }
    }
    ALT_INLINE bool tryLock()
    {
        return !flag_.test_and_set(std::memory_order_acquire);
    }
    ALT_INLINE void unlock()
    {
        flag_.clear(std::memory_order_release);
    }
  private:
    SpinMutex(const SpinMutex&) = delete;
    SpinMutex(SpinMutex&&) = delete;
	SpinMutex& operator = (const SpinMutex&) = delete;
	SpinMutex& operator = (SpinMutex&&) = delete;
};

using ScopedNoneLock = std::scoped_lock<MutexNone>;
using ScopedMutexLock = std::scoped_lock<std::mutex>;
using ScopedSpinLock = std::scoped_lock<SpinMutex>;

}