#include "TimerQueue.h"                      // for TimerQueue, this class

namespace alt {

TimerQueue::TimerQueue()
    : time_queue_()
    , pending_time_queue_()
{
}

void TimerQueue::add
     ( int64_t id, TimeEventListener *listener, const void *event_data,
       Clock::tick_type expire_time, Clock::tick_type interval
     )
{
    auto tail = time_queue_.back();
    while (tail && tail->value_.expire_time_ > expire_time)
    {
        tail = tail->prev();
    }
    auto node = time_queue_.emplaceAfter(tail, id, expire_time, interval, listener, event_data);
    id_node_map_.emplace(id, node);
}

int64_t TimerQueue::addPending
     ( TimeEventListener *listener, const void *event_data,
       Clock::tick_type initial_delay, Clock::tick_type interval
     )
{
    int64_t id = timer_id_.fetch_add(1);
    std::scoped_lock<std::mutex> lock (pending_mutex_);
    pending_time_queue_.emplaceBack(id, initial_delay, interval, listener, event_data);
    return id;
}

void TimerQueue::mergePending ()
{
    std::scoped_lock<std::mutex> lock (pending_mutex_);
    Clock::tick_type time_now = Clock::steadyTicksRaw();
    for (auto const& node: pending_time_queue_)
    {
        const TimeEvent& te = static_cast<TimeEventQueue::NodeType*>(node)->value_;
        add (te.timer_id_, te.listener_, te.event_data_, time_now+te.expire_time_, te.repeat_interval_);
    }
    pending_time_queue_.clear();
}

TimerQueue::TimeEventQueue::NodeType* TimerQueue::reposition (TimeEventQueue::NodeType* node)
{
    TimeEventQueue::NodeType* next_node = time_queue_.extract(node);
    auto tail = time_queue_.back();
    while (tail && tail->value_.expire_time_ > node->value_.expire_time_)
    {
        tail = tail->prev();
    }
    time_queue_.append(tail, node);
    return next_node;
}

TimerQueue::TimeEventQueue::NodeType* TimerQueue::remove (TimeEventQueue::NodeType* node)
{
    id_node_map_.erase(node->value_.timer_id_);
    return time_queue_.erase(node);
}

void TimerQueue::check(Clock::tick_type time_now)
{
    for (TimeEventQueue::NodeType* node = time_queue_.front(); node;)
    {
        TimeEvent& te = node->value_;
        if (te.expire_time_ <= time_now)
        {
            te.listener_->onTimeout(te.timer_id_, te.event_data_);
            if (te.repeat_interval_ > 0)
            {
                Clock::tick_type new_expire_time = te.expire_time_ + te.repeat_interval_;
                if (new_expire_time < time_now)
                {
                    // fire immediately in the next check
                    new_expire_time = time_now + 1;
                }
                node = reposition(node);
            }
            else
            {
                node = remove(node);
            }
        }
        else
        {
            break;
        }
    }
}

Clock::tick_type TimerQueue::nextTimeout(Clock::tick_type ticks_now) const
{
    const TimeEventQueue::NodeType* node = time_queue_.front();
    return node ? node->value_.expire_time_ - ticks_now : -1;
}

int64_t TimerQueue::addTimer
     ( TimeEventListener *listener, const void *event_data,
       Clock::tick_type initial_delay, Clock::tick_type interval, Clock::tick_type time_now
     )
{
    if (listener->isIncipient())
    {
        return addPending(listener, event_data, initial_delay, interval);
    }
    
    int64_t id = timer_id_.fetch_add(1);
    Clock::tick_type expire_time = time_now ? time_now + initial_delay
                                            : Clock::steadyTicksRaw() + initial_delay;
    add(id, listener, event_data, expire_time, interval);
    return id;
}

int TimerQueue::resetInterval
    ( int64_t timer_id,
      Clock::tick_type new_interval
    )
{
    EventIdNode* n = id_node_map_.findValue(timer_id);
    if (n)
    {
        n->event_node_->value_.repeat_interval_ = new_interval;
        return 0;
    }
    return -1;
}

int TimerQueue::delTimer (int64_t timer_id)
{
    auto ev_iter = id_node_map_.find(timer_id);
    if (ev_iter!=id_node_map_.end())
    {
        remove(ev_iter.getValue().event_node_);
        return 0;
    }
    return -1;
}

}  // namespace alt