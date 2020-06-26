#include <util/system/TimerQueue.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <assert.h>
#include <cstring>
#include <time.h>
#include <unistd.h>
//#include <util/system/SysError.h>
//#include <exception>

namespace alt {
class MyTimeEventListener: public TimeEventListener
{
  public:
    void onTimeout(int64_t timer_id, const void * event_data) override
    {
        ++timeout_cnt_;
        current_timer_id_ = timer_id;
    }

    int timeoutCount() const { return timeout_cnt_; }
    int64_t curTimerId() const { return current_timer_id_; }

  private:
    int timeout_cnt_ = 0;
    int64_t current_timer_id_ = 0;

};

} // namespace alt

TEST_CASE( "TimerQueue Test", "[TimerQueue]" ) {
    alt::TimerQueue timer_queue;
    alt::MyTimeEventListener listener;
    int sleep_cnt = 0;
    timer_queue.addTimer(&listener, nullptr, alt::Clock::one_millisec, 0);
    usleep(1100); ++sleep_cnt;
    timer_queue.check(alt::Clock::steadyTicksRaw());
    REQUIRE(listener.timeoutCount() == 1);
    REQUIRE(listener.curTimerId() == 0);
    timer_queue.addTimer(&listener, nullptr, alt::Clock::one_millisec, 0);
    usleep(1100);  ++sleep_cnt;
    timer_queue.addTimer(&listener, nullptr, alt::Clock::one_millisec, alt::Clock::one_millisec);
    timer_queue.check(alt::Clock::steadyTicksRaw());
    REQUIRE(listener.timeoutCount() == 2);
    REQUIRE(listener.curTimerId() == 1);
    usleep(1100); ++sleep_cnt;
    timer_queue.check(alt::Clock::steadyTicksRaw());
    REQUIRE(listener.timeoutCount() == 3);
    REQUIRE(listener.curTimerId() == 2);
    usleep(1100); ++sleep_cnt;
    timer_queue.check(alt::Clock::steadyTicksRaw());
    REQUIRE(listener.timeoutCount() == 4);
    REQUIRE(listener.curTimerId() == 2);    // repeat timeout id 2
    timer_queue.delTimer(2);
    usleep(1100); ++sleep_cnt;
    timer_queue.check(alt::Clock::steadyTicksRaw());
    REQUIRE(listener.timeoutCount() == 4);  // timeoutCount the same, repeatitive timer 2 deleted
    int res = timer_queue.delTimer(2);
    REQUIRE(res == -1); 
}




