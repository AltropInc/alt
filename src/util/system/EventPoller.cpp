#include "EventPoller.h"        // for FDEventPoller, this class
#include "Platform.h"           // for ALT_UNDERLYING_OS
#include "SysError.h"           // for SYS_ERR_THROW

#include <mutex>                // for mutext and scoped_lock
#include <vector>               // for vector
#include <unordered_set>        // for unordered_set
#include <unordered_map>        // for unordered_map
#include <algorithm>            // for swap

#include <time.h>               // for nanosleep
#include <unistd.h>             // for usleep

#if (ALT_UNDERLYING_OS == ALT_OS_LINUX)
#include <sys/epoll.h>
#elif (ALT_OS_FAMILY == ALT_OS_FAMILY_WINDOWS)
#include <util/storage/PooledHash.h>    // for PooledHash
#define _WIN32_WINNT  0x0700
#include <winsock2.h>                   // for poll
inline int poll (struct pollfd *fds, unsigned int nfds, int timeout)
{
    return WSAPoll(fds, (ULONG)nfds, (INT)timeout);
}
#else
#include <util/storage/PooledHash.h>    // for PooledHash
#include <poll.h>                       // for poll
#endif

namespace alt {

//------------------------------------------------------------------------------------
// Implementation using epoll
//------------------------------------------------------------------------------------
#if (ALT_UNDERLYING_OS ==ALT_OS_LINUX)
//#if (1)

class FDEventPoller::Impl
{
  public:
	Impl(bool busy_poller);
	~Impl();
	void book(FDEventHandler* handler, FDEventIdSet event_ids);
	void remove(FDEventHandler* handler);
	//bool has(FDEventHandler* handler) const;
    bool empty() const { return event_cnt_==0; };
	void clear();
	void poll(Clock::tick_type tick, Clock::tick_type timeout);

  private:
	int                                 epoll_id_ {-1};
	std::vector<struct epoll_event>     events_;
    size_t                              event_cnt_{0};
    bool                                busy_poller_;
};

FDEventPoller::Impl::Impl(bool busy_poller)
    : events_ (1024)
    , busy_poller_(busy_poller)
{
    epoll_id_  = epoll_create(1);
    if (epoll_id_ < 0)
    {
        SYS_ERR_THROW(SysException);
    }
}

FDEventPoller::Impl::~Impl()
{
    if (epoll_id_ >= 0)
    {
        ::close(epoll_id_);
    }
}

void FDEventPoller::Impl::book(FDEventHandler* handler, FDEventIdSet event_ids)
{
    FdId fd = handler->fd();
    struct epoll_event ev;
    ev.events = 0;
    if (event_ids.has(FDEventId::EVENT_IN)) ev.events |= EPOLLIN;
    if (event_ids.has(FDEventId::EVENT_OUT)) ev.events |= EPOLLOUT;
    ev.data.ptr = handler;
    // use ev.data.u32 to save original events
    ev.data.u32 = ev.events;

    int rs = epoll_ctl(epoll_id_, EPOLL_CTL_ADD, fd, &ev);

    bool is_mod {false};
    if (rs=EEXIST )
    {
        rs = epoll_ctl(epoll_id_, EPOLL_CTL_MOD, fd, &ev);
        is_mod = true;
    }

    if (rs!=0)
    {
        SYS_ERR_THROW(SysException);
    }

    if (!is_mod)
    {
        ++event_cnt;
    }
}

void FDEventPoller::Impl::remove(FDEventHandler* handler)
{
    struct epoll_event ev {0,0};
    int err = epoll_ctl(epoll_id_, EPOLL_CTL_DEL, handler->fd(), &ev);
    if (err)
    {
        SYS_ERR_THROW(SysException);
    }
    --event_cnt_;
}

void FDEventPoller::Impl::clear()
{
    ::close(epoll_id_);
    epoll_id_ = epoll_create(1);
    if (epoll_id_ < 0)
    {
        SYS_ERR_THROW(SysException);
    }
    event_cnt_ =0;
}

void FDEventPoller::Impl::poll(Clock::tick_type tick, Clock::tick_type timeout)
{
    int rc;
#if 0
    Clock::duration time_left(timeout);
    while (true)
    {
        Clock::time_point start_poll_time;
        rc = epoll_wait(epoll_id_, &events_[0], events_.size(), Clock::millisecs(time_left));
        if (rc >= 0 || errno != EINTR)
        {
            break;
        }
        Clock::time_point now;
        Clock::duration waited_time = now - start_poll_time;
        time_left = waited_time < time_left ? time_left - waited_time : Clock::duration(0);
    }
#endif

    rc = epoll_wait(epoll_id_, &events_[0], events_.size(), Clock::millisecs(time_left));

    if (rc < 0 && errno != EINTR)
    {
        SYS_ERR_THROW(SysException);
    }

    for (auto& e : events_)
    {
        FDEventIdSet event_ids;
        if (e.events & EPOLLIN) event_ids |= FDEventId::EVENT_IN;
        if (e.events & EPOLLOUT) event_ids |= FDEventId::EVENT_OUT;
        if (e.events & EPOLLERR) event_ids |= FDEventId::EVENT_ERROR;
        if (!event_ids.empty())
        {
            FDEventIdSet done_set = reinterpret_cast<FDEventHandler*>(e.data.ptr)->
                onEvent(tick, event_ids);
            uint32_t interested_events = e.data.u32;
            if (done_set.has(FDEventId::EVENT_IN)) interested_events &= ~EPOLLIN;
            if (done_set.has(FDEventId::EVENT_OUT)) interested_events &= ~EPOLLOUT;
            if (e.data.u32==0)
            {
                // the handler is no longer intersted in any event
                remove(reinterpret_cast<FDEventHandler*>(e.data.ptr));
            }
            else if (interested_events!=e.data.u32)
            {
                // modify to intersted event set
                e.events = interested_events;
                e.data.u32 = interested_events;
                epoll_ctl(epoll_id_,
                    EPOLL_CTL_MOD,
                    reinterpret_cast<FDEventHandler*>(e.data.ptr)->fd(),
                    &e);
            }
        }
    }
}

#else
//------------------------------------------------------------------------------------
// Implementation using poll
//------------------------------------------------------------------------------------
class FDEventPoller::Impl
{
  public:

    Impl(bool busy_poller);

	void poll(Clock::tick_type tick, Clock::tick_type timeout);
	void book(FDEventHandler* handler, FDEventIdSet event_ids);
	void remove(const FDEventHandler* handler);
	bool has(const FDEventHandler* handler) const;
	bool empty() const;
	void clear();

  private:

    void updatePollFDs();

    struct EventIndex
    {
        FdId            fd_;
        size_t          index_;

        EventIndex(FdId fd, size_t index)
          : fd_(fd), index_(index)
        {}

        MAKE_POOLED_HASH_ENTRY(FdId, fd_, size_t);
    };

    // a fast hash map from fd to event index without dynamic memory allocation
    using EventIndexMap = PooledHash<EventIndex>;
    EventIndexMap                               event_index_map_;
	std::vector<pollfd>                         poll_fds_;
    std::vector<FDEventHandler*>                handlers_;
    size_t                                      event_num_{0};
	std::mutex                                  mutex_;
    bool                                        busy_poller_;
};

FDEventPoller::Impl::Impl(bool busy_poller)
    : poll_fds_(1024)
    , handlers_(1024, nullptr)
    , busy_poller_(busy_poller)

{};

void FDEventPoller::Impl::book(FDEventHandler* handler, FDEventIdSet event_ids)
{
    std::scoped_lock(mutex_);

    FdId fd = handler->fd();
    EventIndex* ev_ix = event_index_map_.findValue(fd);

    short events = 0;
    if (event_ids.has(FDEventId::EVENT_IN)) events |= POLLIN;
    if (event_ids.has(FDEventId::EVENT_OUT)) events |= POLLOUT;

    if (ev_ix)
    {
        poll_fds_[ev_ix->index_].events = events;
        handlers_[ev_ix->index_] = handler;
    }
    else if (event_num_ < poll_fds_.size())
    {
        poll_fds_[event_num_] = {fd, events, 0};
        handlers_[event_num_] = handler;
        event_index_map_.insert({fd, event_num_});
        ++event_num_;
    }
    else
    {
        poll_fds_.push_back({fd, events, 0});
        handlers_.push_back(handler);
        event_index_map_.insert({fd, poll_fds_.size()-1});
        ++event_num_;
    }    
}

void FDEventPoller::Impl::remove(const FDEventHandler* handler)
{
    std::scoped_lock(mutex_);

    FdId fd = handler->fd();
    auto ev_iter = event_index_map_.find(fd);
    if (ev_iter!=event_index_map_.end())
    {
        auto& ev_ix = ev_iter.getEntry()->value_;
        if (event_num_)
        {
            --event_num_;
            handlers_[ev_ix.index_] = nullptr;
            if (ev_ix.index_ < event_num_)
            {
                std::swap(poll_fds_[ev_ix.index_], poll_fds_[event_num_]);
                std::swap(handlers_[ev_ix.index_], handlers_[event_num_]);
            }
        }
        event_index_map_.erase(ev_iter);
    }
}

void FDEventPoller::Impl::clear()
{
    std::scoped_lock(mutex_);
    event_index_map_.clear();
    event_num_ = 0;
}

void FDEventPoller::Impl::poll(Clock::tick_type tick_realtime, Clock::tick_type timeout)
{
    if (event_num_==0) return;

    int rc;

    // system poll timeout is in milliseconds.
    if (timeout > 0 && timeout < Clock::one_millisec)
    {
        // a simulation to timeout less than one millisecond. If this is not
        // a busy poller, it uses nanosleep so the timeout may be much longer because
        // of the yield of the thread. Therefore, is not recommended to use a
        // timeout smaller than a millisecond for non-busy poller
        Clock::tick_type start_poll_tick = Clock::steadyTicksRaw();
        Clock::tick_type sleep_ticks = timeout >> 3;
        while (true)
        {
            // pool without timeout
            rc = ::poll(&poll_fds_[0], event_num_, 0);
            if (rc==0)
            {
                // if there is no event
                Clock::tick_type now = Clock::steadyTicksRaw();
                Clock::tick_type waited_time = now - start_poll_tick;
                if (waited_time > timeout)
                {
                    break;
                }
                if (busy_poller_)
                {
                    while (true)
                    {
                        Clock::tick_type now2 = Clock::steadyTicksRaw();
                        if (now2-now > sleep_ticks)
                        {
                            break;
                        }
                    }
                }
                else
                {
                    if (ALT_LIKELY(sleep_ticks < Clock::one_sec))
                    {
                        // sleep one eighth of the timeout before the next pool
                        struct timespec  req {0, sleep_ticks};
                        struct timespec  rem ;
                        ::nanosleep(&req, &rem);
                    }
                    else
                    {
                        ::usleep(sleep_ticks/1000);
                    }
                }

                continue;
            }
            break;
        }
    }
    else
    {
        rc = ::poll(&poll_fds_[0], event_num_, Clock::millisecs(timeout));
    }

    if (rc < 0 && errno != EINTR)
    {
        SYS_ERR_THROW(SysException);
    }

    for (size_t i=0; i < event_num_; ++i)
    {
        auto & e = poll_fds_[i];
        auto handler = handlers_[i];
        if (handler)
        {
            FDEventIdSet event_ids;
            if (e.revents & POLLIN) event_ids |= FDEventId::EVENT_IN;
            if (e.revents & POLLOUT) event_ids |= FDEventId::EVENT_OUT;
            if (e.revents & POLLERR) event_ids |= FDEventId::EVENT_ERROR;
#if (ALT_UNDERLYING_OS ==ALT_OS_WINDOWS)
            if (e.revents & POLLHUP) event_ids |= FDEventId::EVENT_IN;
#endif
            if (!event_ids.empty())
            {
                FDEventIdSet done_set = handler->onEvent(tick_realtime, event_ids);
                if (done_set.has(FDEventId::EVENT_OUT))
                {
                    // handler is no longer interested in EVENT_OUT
                    e.events &= ~POLLOUT;
                }
                if (done_set.has(FDEventId::EVENT_IN))
                {
                    // handler is no longer interested in EVENT_IN
                    e.events &= ~POLLIN;
                }
                if (e.events==0)
                {
                    // handler is no longer interested any event
                    // remove this event in poll_fds_
                    std::swap (e, poll_fds_[--event_num_]);
                    --i;
                }
            } 
        }
    }
}
#endif

FDEventPoller::FDEventPoller(bool busy_poller)
{
    impl_.reset(new Impl(busy_poller));
}

FDEventPoller::~FDEventPoller()
{
}

void FDEventPoller::book(FDEventHandler* handler, FDEventIdSet event_ids)
{
    impl_->book(handler, event_ids);
}

void FDEventPoller::remove(const FDEventHandler* handler)
{
    impl_->remove(handler);
}

/*
bool FDEventPoller::has(const FDEventHandler* handler) const
{
    return impl_->has(handler);
}
*/

bool FDEventPoller::empty() const
{
    return impl_->empty();
}

void FDEventPoller::clear()
{
    impl_->clear();
}

void FDEventPoller::poll(Clock::tick_type tick, Clock::tick_type timeout)
{
    impl_->poll(tick, timeout);
}


} // namespace alt