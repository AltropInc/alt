#include "Reactor.h"            // for Reactor, this class

namespace alt {


Reactor::Reactor(ReactorOwner* owner )
    : owner_(owner)
{}

TimerQueue & Reactor::getTimerQueue()
{
    if (!timer_queue_)
    {
        timer_queue_.reset(new TimerQueue());
    }
    return *timer_queue_;
}

void Reactor::createThreadMsgPoller (CoQueueMsgHandler& handler, int max_poll_num, bool use_pool)
{
    if (thread_msg_poller_ < 0)
    {
        if (use_pool)
        {
            auto poller = new CoQueueMsgPoller<Allocator>(handler, max_poll_num);
            thread_msg_poller_ = addMessagePoller(poller);
        }
        else
        {
            auto poller = new CoQueueMsgPoller<PooledAllocator>(handler, max_poll_num);
            thread_msg_poller_ = addMessagePoller(poller);
        }
        thread_msg_poller_use_pool_ = use_pool;
    }
}

void Reactor::setEventPoller(EventPoller* event_poller)
{
    event_poller_.reset(event_poller);  
}

int Reactor::addMessagePoller(MessagePoller* msg_poller)
{
    size_t sz = message_pollers_.size();
    message_pollers_.resize(sz+1);
    message_pollers_[sz].reset(msg_poller);
    return int(sz);
}

MessagePoller * Reactor::getMessagePoller(int id)
{
    return id >=0 && id < message_pollers_.size() ? message_pollers_[id].get() : nullptr;
}

void Reactor::stop()
{
    stop_flag_.store(true, std::memory_order_release);
}

bool Reactor::stopoed() const
{
    return stopped_.load();
}

bool Reactor::running() const
{
    return running_.load();
}

void Reactor::run(
    Clock::tick_type poll_interval,
    bool busy_poller,
    bool power_saving,
    Clock::tick_type max_poll_timeout)
{
    poll_interval_    = poll_interval;
    busy_poller_      = busy_poller;
    power_saving_     = power_saving;
    max_poll_timeout_ = max_poll_timeout;

    Clock::ClockTicks ticks;

    running_.store(true);

    while (true)
    {
        if (stop_flag_.load(std::memory_order_relaxed))
        {
            if (owner_) owner_->onReactorStop();
            break;
        }

        Clock::clockTicks(ticks);

        if (timer_queue_)
        {
            timer_queue_->check(ticks.ticks_raw_);
        }

        for (auto& msg_poller: message_pollers_)
        {
            msg_poller->poll(ticks.ticks_since_epoch_);
        }

        if (event_poller_)
        {
            if (isBusyPoller())
            {
                // For busy poller, do not consider any power saving options. Simply use poll_interval_
                // for the timeout in polling the event.
                event_poller_->poll(ticks.ticks_since_epoch_, poll_interval_);
            }
            else
            {
                Clock::tick_type poll_timeout = poll_interval_;
                Clock::tick_type next_timeout = timer_queue_ ? timer_queue_->nextTimeout(ticks.ticks_raw_) : -1;
                if ((power_saving_ || message_pollers_.empty()) && next_timeout != 0)
                {
                    // If we do not have any message poller and the next_timeout is greater than
                    // poll_interval_, we can use the larger timeout value to poll the event.
                    // This will put this thread in a longer wait period if there is no event in the
                    // event poller, which may help in saving the CPU power.
                    if (next_timeout < 0)
                    {
                        next_timeout = max_poll_timeout_;
                    }
                    poll_timeout = std::max(next_timeout, poll_timeout);
                }
                event_poller_->poll(ticks.ticks_since_epoch_, poll_timeout);
            }
        }
    }

    running_.store(false);
    stopped_.store(true);
}

#if 0

#include <w32api/windows.h>             // for Windows API
#include <thread>                       // for thread

class WindowsMessageHandler
{
  public:
     virtual void processMessage(Clock::tick_type tick, MSG& msg) =0;
        // appContext->UpdateApplication(); //CS40
        // TranslateMessage(&msg);
        // DispatchMessage(&msg);
};


class WindowsMessagePoller : public EventPoller
{
  public:
    WindowsMessagePoller(WindowsMessageHandler& msg_handler);

    ~WindowsMessagePoller();

    void poll(Clock::tick_type tick, Clock::tick_type poll_timeout) override;

    void setPollTimeout(Clock::tick_type ticks);
    void stop();

  private:
    void pollerTimeoutFeeder();

    DWORD poller_thread_id_;
    std::atomic<int64_t> poll_timeout_;
    std::atomic<bool> stop_flag_{false};
    std::thread timeout_feed_thread_;
    WindowsMessageHandler & msg_handler_;
};

void WindowsMessagePoller::setPollTimeout(Clock::tick_type ticks)
{
    // poll_timeout_ is in 100 nanosecond intervals
    poll_timeout_.store(ticks/100, std::memory_order_release);
}

void WindowsMessagePoller::stop()
{
    stop_flag_.store(true, std::memory_order_release);
}

void WindowsMessagePoller::pollerTimeoutFeeder()
{
    HANDLE hTimer = NULL;

    // Create an unnamed waitable timer.
    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (NULL == hTimer)
    {
        PostThreadMessage(poller_thread_id_, WM_TIMER, -1 , GetLastError());
        return;
    }

    LARGE_INTEGER time_to_wait;

    // run thread feeder
    while (true)
    {
        if (stop_flag_.load(std::memory_order_acquire))
        {
            break;
        }
        // This is in 100 nanosecond intervals, negative means relative the current
        time_to_wait.QuadPart = -poll_timeout_.load(std::memory_order_acquire);

        // Set a timer to wait for poll_timeout_.
        if (!SetWaitableTimer(hTimer, &time_to_wait, 0, NULL, NULL, 0))
        {
            PostThreadMessage(poller_thread_id_, WM_TIMER, -1 , GetLastError());
            return;
        }

        // wait for the timer
        if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
        {
            PostThreadMessage(poller_thread_id_, WM_TIMER, -1 , GetLastError());
        }
        else
        {
            PostThreadMessage(poller_thread_id_, WM_TIMER, 0 , 0);
        }
    }
}

WindowsMessagePoller::WindowsMessagePoller(WindowsMessageHandler& msg_handler)
    : timeout_feed_thread_(&WindowsMessagePoller::pollerTimeoutFeeder, *this)
    , msg_handler_(msg_handler)
{
}

WindowsMessagePoller::~WindowsMessagePoller()
{
}

void WindowsMessagePoller::poll(Clock::tick_type tick, Clock::tick_type poll_timeout)
{
    setPollTimeout(poll_timeout);
    MSG msg;
    GetMessage(&msg, NULL, 0, 0);
    if (msg.message==WM_TIMER)
    {
        // timeout, no message
        if (msg.message,msg.wParam==-1)
        {
            SYS_ERR_THROW();
        }
        return;
    }
    msg_handler_.processMessage(tick, msg);
}
#endif

}


#if 0

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Display *dis;
Window win;
int x11_fd;
fd_set in_fds;

struct timeval tv;
XEvent ev;

int main() {
    dis = XOpenDisplay(NULL);
    win = XCreateSimpleWindow(dis, RootWindow(dis, 0), 1, 1, 256, 256, \
        0, BlackPixel (dis, 0), BlackPixel(dis, 0));

    // You don't need all of these. Make the mask as you normally would.
    XSelectInput(dis, win, 
        ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask |
        ButtonPressMask | ButtonReleaseMask  | StructureNotifyMask 
        );

    XMapWindow(dis, win);
    XFlush(dis);

    // This returns the FD of the X11 display (or something like that)
    x11_fd = ConnectionNumber(dis);

    // Main loop
    while(1) {
        // Create a File Description Set containing x11_fd
        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        // Set our timer.  One second sounds good.
        tv.tv_usec = 0;
        tv.tv_sec = 1;

        // Wait for X Event or a Timer
        int num_ready_fds = select(x11_fd + 1, &in_fds, NULL, NULL, &tv);
        if (num_ready_fds > 0)
            printf("Event Received!\n");
        else if (num_ready_fds == 0)
            // Handle timer here
            printf("Timer Fired!\n");
        else
            printf("An error occured!\n");

        // Handle XEvents and flush the input 
        while(XPending(dis))
            XNextEvent(dis, &ev);
    }
    return(0);
}

#endif