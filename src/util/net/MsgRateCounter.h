#pragma once
#include <util/system/Platform.h>
#include <util/Intrinsics.h>
#include <vector>

namespace alt {

using Time_t = int64_t;
constexpr Time_t ONE_SEC = 1000000000LL;

class MsgRateCounter
{
    size_t           slot_number_;
    std::vector<int> slot_counter_;
    size_t           slot_mask_;
    size_t           slot_shift_bits_;
    size_t           sliding_window_start_;
    size_t           counter_;

  public:
    MsgRateCounter(size_t slot_number)
        : slot_number_(power2Next<size_t>(slot_number))
        , slot_counter_(slot_number, 0)
        , slot_mask_(slot_number_-1)
        , slot_shift_bits_(log2Floor(slot_number_))
        , sliding_window_start_(0)
        , counter_(0)
    {}

    size_t count(Time_t t, int msgs)
    {
        size_t time_slot = t >> slot_shift_bits_;
  
        size_t new_sliding_window_start = time_slot - slot_number_ + 1;
        size_t max_slots_to_check = std::min(new_sliding_window_start, sliding_window_start_+slot_number_);
        
        for (size_t ix = sliding_window_start_; ix < max_slots_to_check; ++ix)
        {
            counter_ -= slot_counter_[ix & slot_mask_];
            slot_counter_[ix] = 0;
        }            

        slot_counter_[time_slot & slot_mask_] += msgs;
        counter_ += msgs;
        sliding_window_start_ = new_sliding_window_start;
        return counter_;
    }
};

}