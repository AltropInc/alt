#include "Clock.h"
#include <time.h>
#include <atomic>
#include <unistd.h>  // for pause

namespace alt
{

using system_clock = std::chrono::system_clock;
using steady_clock = std::chrono::steady_clock;

class Clock::Impl
{
  public:
    virtual void simStart (time_point starting_time) {}
    virtual void simAdvance(uint64_t ticks) {};
    virtual Clock::tick_type ticks() =0;
    virtual void clockTicks(ClockTicks &) {};
    virtual time_point now() =0;
};

inline Clock::tick_type getRealTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec*1000000000L + ts.tv_nsec;
}

inline Clock::tick_type getSteadyTime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*1000000000L + ts.tv_nsec;
}

class ClockRealtime: public Clock::Impl
{
  public:
    ClockRealtime() {}
    Clock::tick_type ticks() override
    {
        return getRealTime();
    }
    Clock::time_point now() override
    {
        return Clock::time_point(Clock::duration(getRealTime()));
    }
};

class ClockSteady: public Clock::Impl
{
  public:
    ClockSteady(Clock::tick_type  calibrate_interval);
    Clock::tick_type ticks() override;
    Clock::time_point now() override;
    Clock::tick_type ticksRaw();
    void clockTicks(Clock::ClockTicks & ticks) override;

  private:
    Clock::tick_type calibrate ();
    std::atomic<bool> calibrating_ {false};
    Clock::tick_type real_time_offset_{0};
    Clock::tick_type calibrate_interval_{Clock::one_sec};
    std::atomic<Clock::tick_type> last_calibrate_time_{0};
    std::atomic<Clock::tick_type> last_tick_{0};
};

class ClockSimulation: public Clock::Impl
{
  public:
    ClockSimulation() {}
    Clock::tick_type ticks() override
    {
        return ticks_;
    }
    Clock::time_point now() override
    {
        return Clock::time_point(Clock::duration(ticks_));
    }
    void simStart(Clock::time_point start_time) override
    {
        ticks_ = start_time.time_since_epoch().count();
    }
    void simAdvance(uint64_t ticks) override
    {
        ticks_ += ticks;
    }
  private:
    Clock::tick_type ticks_{0};
};

ClockSteady::ClockSteady(Clock::tick_type calibrate_interval)
    : calibrate_interval_(calibrate_interval)
{
    calibrate();
}

Clock::tick_type ClockSteady::calibrate ()
{
    Clock::tick_type steady_epoch0 = getSteadyTime();
    Clock::tick_type system_epoch = getRealTime();
    Clock::tick_type steady_epoch = getSteadyTime();
    steady_epoch =  (steady_epoch0 + steady_epoch)/2;
    real_time_offset_ = system_epoch - steady_epoch;

    // when calibrating, no other thread has access to last_tick_
    if (steady_epoch + real_time_offset_ <= last_tick_)
    {
        // make sure steady_epoch is not going back after calibrate
        steady_epoch = last_tick_ - real_time_offset_ + 1;
    }
    last_calibrate_time_.store(steady_epoch, std::memory_order_release);
    return steady_epoch;
}

Clock::tick_type ClockSteady::ticksRaw()
{
    Clock::tick_type steady_epoch = getSteadyTime();
    if (steady_epoch - last_calibrate_time_.load(std::memory_order_acquire) >
        calibrate_interval_)
    {
        bool calibrating_status {false};
        if (!calibrating_.compare_exchange_strong(
                    calibrating_status, true,
                    std::memory_order_acq_rel, std::memory_order_relaxed))
        {
            // someone else is doing calibrating, do nothing and just wait for
            // calibrating finish
            while (calibrating_.load(std::memory_order_acquire))
            {
                // Not a really pause, just tell the cpu that we are spinning
                pause();
            }
            steady_epoch = getSteadyTime();
        }
        else
        {
            steady_epoch = calibrate();
            calibrating_.store(false, std::memory_order_release);
        }
    }
    return steady_epoch;
}

Clock::tick_type ClockSteady::ticks()
{
    Clock::tick_type steady_epoch = ticksRaw();
    steady_epoch += real_time_offset_;
    last_tick_.store(steady_epoch, std::memory_order_release);
    return steady_epoch;
}

void ClockSteady::clockTicks(Clock::ClockTicks & ticks)
{
    ticks.ticks_raw_ = ticksRaw();
    ticks.ticks_since_epoch_ =  ticks.ticks_raw_ + real_time_offset_;
    last_tick_.store(ticks.ticks_since_epoch_, std::memory_order_release);
}

Clock::time_point ClockSteady::now()
{
    return Clock::time_point(Clock::duration(ticks()));
}

void Clock::init(
        ClockType clock_type,
        tick_type calibrate_interval)
{
    if (impl_)
    {
        // Do not initialize it twice
        return;
    }

    default_clock_type_ = clock_type;

    switch (default_clock_type_)
    {
        case ClockType::RealTime:
            impl_.reset(new ClockRealtime());
            is_steady = false;
            break;

        case ClockType::Steady:
        case ClockType::HighResolution:  // HighResolution needs to be implemented
            impl_.reset(new ClockSteady(calibrate_interval));
            is_steady = true;
            break;

        case ClockType::Simulation:
            impl_.reset(new ClockSimulation());
            is_steady = true;
            break;
    }
}

Clock::time_point Clock::now()
{
    return impl_->now();
}

void Clock::simStart (time_point starting_time)
{
    return impl_->simStart(starting_time);
}

void Clock::simAdvance(uint64_t ticks)
{
    return impl_->simAdvance(ticks);
}

Clock::tick_type Clock::realtimeTicks()
{
    return getRealTime();
}

Clock::tick_type Clock::steadyTicksRaw()
{
    return getSteadyTime();
}

Clock::tick_type Clock::ticks()
{
    return impl_->ticks();
}

Clock::tick_type Clock::steadyTicks()
{
    if (Clock::default_clock_type_==ClockType::Steady ||
        Clock::default_clock_type_==ClockType::HighResolution)
    {
        return impl_->ticks();
    }
    if (!steady_impl_)
    {
        steady_impl_.reset(new ClockSteady(calibrate_interval_));
    }
    return steady_impl_->ticks();
}

void Clock::clockTicks(ClockTicks & ticks)
{
    if (Clock::default_clock_type_==ClockType::Steady ||
        Clock::default_clock_type_==ClockType::HighResolution)
    {
        impl_->clockTicks(ticks);
        return;
    }
    ticks.ticks_since_epoch_ = impl_->ticks();
    ticks.ticks_raw_ = getSteadyTime();
}

bool Clock::is_steady;
ClockType Clock::default_clock_type_;
Clock::tick_type Clock::calibrate_interval_;
OpaquePointer<Clock::Impl> Clock::impl_;
OpaquePointer<Clock::Impl> Clock::steady_impl_;

}