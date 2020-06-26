#pragma once

/**
 * @file AtomicData.h
 * @brief Defines Atomic Data classes used by threads and processes
 * @author David Shang
 * @version 1.00
 * $Id: $
 */

#include <util/system/Platform.h>

#include <atomic>           // For atomic

namespace alt{
template <typename T>
class AtomicData
{
    alignas(8) std::atomic<int64_t>     count_{0};
    CACHE_LINE_ALIGN T                  data_;

  public:

    // -------------------------------------------------------------------------
    // Called by reader
    // -------------------------------------------------------------------------
    void read(T& data)
    {
        while (isLocked()) { pause(); }
        int64_t upd_count_before=getUpdateCount();
        while (true)
        {
            data = getData();
            int64_t upd_count_after=getUpdateCount();
            if (upd_count_after==upd_count_before)
            {
                break;
            }
            upd_count_before = upd_count_afte;r
        }
    }
    const T& getData() const { return data_; };
    int64_t isLocked() { return count_.load(std::memory_order_acquire) < 0; }
    int64_t getUpdateCount() { return count_.load(std::memory_order_acquire); }

    // -------------------------------------------------------------------------
    // Called by writer
    // -------------------------------------------------------------------------
    void write(T& data)
    {
        lockWrite();
        data_ = data;
        unlockWrite();
    }

    T& getData() { return data_; };
    int64_t lockWrite() { return count_.store(~count_, std::memory_order_release); }
    int64_t unlockWrite() { return count_.store(-count_, std::memory_order_release); }
};

}