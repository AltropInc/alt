#pragma once

#include <util/types/OpaquePointer.h>
#include <chrono>

namespace alt
{

enum class ClockType
{
    /// \brief a real-time wall-clock, that may be adjusted forward or
    /// backward to be synchronized with NTP. If the application does not care
    /// about the performance mesurement, use this clock type, which represents
    /// the best-guess as to the real-time wall-clock.
    RealTime,

    /// \brief a steady (monotonic) to represent wall-clock. It cannot be adjusted
    /// backward but the forwarding speed can be calibrated in order to synchronize
    /// with real-time wall-clock. If your application cares about the performance
    /// mesurement or uses time point for a sequential timestamp, use this. The time
    /// point may drift away a few nano/micro seconds in a short period from the
    /// real-time wall-clock because it does not adjust the time right away. Steady
    /// clock is typically more efficient than real-time clock.
    Steady,

    /// \brief a high resolution steady clock using TSC register to represent a wall-
    /// clock. This is the clock with the least overhead and the highest resolution.
    /// If the platform does not support TSC, a steady monotonic clock will be used.
    /// This clock is CPU dependent, and can only be accessed through one (typically
    /// the main) thread running on a dedicated CPU. Other threads in the application
    /// should use the time value passed from the main thread or use other clock
    /// than the default to get its independent time point for its own purpose.
    /// This is to be implemented
    HighResolution,

    /// \brief a simulation clock gives time point relative to a simulation start
    /// time, and is explicitly controlled by the simulation application, i.e. it
    /// goes forward only by calling simAdvance from the application
    Simulation,
};

/**
 * \class Clock
 * \brief Represents the clock used across all ALT libraries and applications
 */
struct Clock
{
    using tick_type    = int64_t;
    using tick_unit    = std::nano;

    //-----------------------------------------------------------------------------
    // static members to make it a std clock
    //-----------------------------------------------------------------------------
    using  duration    = std::chrono::duration<tick_type, tick_unit>;
    using  rep         = duration::rep; 
    using  period      = duration::period; 
    using  time_point  = std::chrono::time_point<Clock, duration>;
    static bool is_steady;

    /// \brief get now time of the default clock
    static time_point now();

    //-----------------------------------------------------------------------------
    // other static members
    //-----------------------------------------------------------------------------

    static constexpr tick_type one_week =            604800000000000L;
    static constexpr tick_type one_day =             86400000000000L;
    static constexpr tick_type one_hour =            3600000000000L;
    static constexpr tick_type one_minute =          60000000000L;
    static constexpr tick_type one_sec =             1000000000L;
    static constexpr tick_type one_millisec =        1000000L;
    static constexpr tick_type one_microsec =        1000L;
    static constexpr tick_type one_nanosec =         1L;

    static constexpr tick_type week(int64_t x) { return x * one_week; }
    static constexpr tick_type day(int64_t x) { return x * one_day; }
    static constexpr tick_type hour(int64_t x) { return x * one_hour; }
    static constexpr tick_type minute(int64_t x) { return x * one_minute; }
    static constexpr tick_type sec(int64_t x) { return x * one_sec; }
    static constexpr tick_type millisec(int64_t x) { return x * one_millisec; }
    static constexpr tick_type microsec(int64_t x) { return x * one_microsec; }
    static constexpr tick_type nanosec(int64_t x) { return x; }

    static constexpr int64_t weeks(tick_type x) { return x/one_week; }
    static constexpr int64_t days(tick_type x) { return x/one_day; }
    static constexpr int64_t hours(tick_type x) { return x/one_hour; }
    static constexpr int64_t minutes(tick_type x) { return x/one_minute; }
    static constexpr int64_t secs(tick_type x) { return x/one_sec; }
    static constexpr int64_t millisecs(tick_type x) { return x/one_millisec; }
    static constexpr int64_t microsecs(tick_type x) { return x/one_microsec; }
    static constexpr int64_t nanosecs(tick_type x) { return x; }

    static constexpr timeval toTimeVal(duration x)
    {
        return {x.count()/one_sec, (x.count()%one_sec)/1000};
    }

    static constexpr duration fromTimeVal(const struct timeval & tv)
    {
        return duration(sec(tv.tv_sec) + microsec(tv.tv_usec));
    }


    /// \brief get the default clock type used across ALT
    static ClockType defaultClockType() { return default_clock_type_; }

    /// \brief initialize the clock
    /// \param clock_type the default clock type used. See comments on ClockType
    /// \param calibrate_interval interval to calibrate the clock with the real-
    /// time wall-clock. This affects Steady and HighFrequency clock only
    static void init(
        ClockType clock_type,
        tick_type  calibrate_interval = one_sec);

    /// \brief get now time in nano ticks of the default clock since UNIX epoch time
    static tick_type ticks();

    /// \brief get nano ticks of the given time point since UNIX epoch time
    static tick_type ticks(time_point tp) { return tp.time_since_epoch().count(); };

    /// \brief get nano ticks of the current time since UNIX epoch time
    static tick_type realtimeTicks();

    /// \brief get a monotonic nano ticks of the current time since the system-
    /// specific epoch time (typically the system start up time excluding the sleep)
    /// \note the returned value cannot be used as a time_point being compared with
    /// the time point of the default clock because of its different epoch time. Use
    /// this in caution.
    static tick_type steadyTicksRaw();

    /// \brief get a monotonic nano ticks of the current time since UNIX epoch time
    static tick_type steadyTicks();

    struct ClockTicks
    {
        tick_type   ticks_since_epoch_;
        tick_type   ticks_raw_;
    };
    /// \brief get a clock nano ticks of the current time since UNIX epoch time
    /// and the raw ticks since the system start up time
    static void clockTicks(ClockTicks & ticks);

    //-----------------------------------------------------------------------------
    // If the clock is not a Simulation clock, the following functions do nothing
    //-----------------------------------------------------------------------------
    /// \brief start the Simulation clock by the given time point.
    static void simStart (time_point starting_time);

    /// \brief advance Simulation the clock ticks by the given ticks.
    static void simAdvance(uint64_t ticks);

    /// \brief opaque implementation
    class Impl;

  private:
    static ClockType default_clock_type_;
    static tick_type calibrate_interval_;

    // implementation of the default clock
    static OpaquePointer<Impl> impl_;

    // implementation of the steady clock. This is created only when the default clock
    // is not steady and steadyTicks is called to ask for monotonic nano ticks of
    // the current time since UNIX epoch time
    static OpaquePointer<Impl> steady_impl_;
};

}