
#include <util/Defs.h>                  // for ALT_UTIL_PUBLIC
#include <util/types/Clock.h>           // for Clock
#include <util/storage/LinkedList.h>    // for FixPooledLinkList
#include <util/storage/PooledHash.h>    // for PooledHash

#include <functional>                   // for function
#include <vector>                       // for vector
#include <mutex>                        // for mutex, scoped_lock

namespace alt {

class TimeEventListener
{
  public:
    /// \brief tells if the listener is in incepient status when it cannot handle any
    /// timeout event yet. For instance, it is in a prealoding status by a different thread.
    /// If the listener calls addTimeEvent in incepient status, the time event will be added
    /// in the pending_time_queue_
    virtual bool isIncipient() const { return false; }

    virtual void onTimeout(int64_t timer_id, const void * event_data) =0;
};

/**
 * \class TimerQueue
 * \ingroup Util
 * \brief A time event (timer) manager per thread sitting within a reactor to manage
 * timeout events. For the purpose of performance and the need to support busy poll
 * running on dedicated core, it is normally not protected by a mutex, assuming it is
 * used only by the thread that owns the reactor. However, if a thread wants to add
 * time events for a listener that will run from a different thread, for instance,
 * an object preloading thread loading and constructing an object to be delivered for
 * a different thread to run, call addPending. After the thread finshed its job,
 * notify the thread that owns the listener to merge the pending events.
 */
class TimerQueue
{
  public:

    /// \brief constructs a time event manager
    TimerQueue();

    /// \brief add a timer
    /// \param listener the object to be called back on timeout
    /// \param event_data pointer to user provided data that will be passed in the callback
    /// \param initial_delay the time span from now to the time when fire the timeout event
    /// \param interval the time span to fire repeatitive timeout events after the initial
    /// timeout event. If this is set to zero, no repeatitive timeout events will follow,
    /// and the timer will be removed after the initial one fired.
    /// \param time_now the current time. If it is not set, the current time is the clock's
    /// current monotonic nano ticks since the system start up time (excluding the sleep
    /// time). If you provide current time, using the time from Clock::steadyTicksRaw() is
    /// recommended. Other clock time scheme does not handle the case of app wakeup from
    /// sleep.
    /// \return the timer id of the timer added. This is the id will be passed in the callback
    int64_t addTimer
        ( TimeEventListener *listener,
          const void *event_data,
          Clock::tick_type initial_delay,
          Clock::tick_type interval,
          Clock::tick_type time_now=0
        );

    /// \brief add a pending timer
    /// \param listener the object to be called back on timeout
    /// \param event_data pointer to user provided data that will be passed in the callback
    /// \param initial_delay the time span from the time when the pending timer becomes
    /// active to the time when fire the timeout event
    /// \param interval the time span to fire repeatitive timeout events after the initial
    /// timeout event. If this is set to zero, no repeatitive timeout events will follow,
    /// and the timer will be removed after the initial one fired.
    /// \return the timer id of the pending timer added. It will be the same timer id when
    /// it becomes active
    int64_t addPending
        ( TimeEventListener *listener,
          const void *event_data,
          Clock::tick_type initial_delay,
          Clock::tick_type interval
        );

    /// \brief activate all pending timers
    void mergePending ();

    /// \brief reset the repeatitive interval. This does not work for pending timers
    /// \param timer_id the id of the timer
    /// \param new_interval the new repeatitive interval
    /// \return 0 if the repeatitive interval is updated, -1 if the timer is not active
    /// or it does not exist.
    int resetInterval
        ( int64_t timer_id,
          Clock::tick_type new_interval
        );

    /// \brief delete the timer. This does not work for pending timers
    /// \param timer_id the id of the timer
    /// \param new_interval the new repeatitive interval
    /// \return 0 if the timer is deleted, -1 if the timer is not active
    /// or it does not exist.
    int delTimer
        ( int64_t timer_id
        );

    /// \brief fire all expired timers
    void check(Clock::tick_type time_now);

    Clock::tick_type nextTimeout(Clock::tick_type ticks_now) const;

  private:

    struct TimeEvent
    {
        int64_t                      timer_id_;
        Clock::tick_type             expire_time_;
        Clock::tick_type             repeat_interval_;
        TimeEventListener *          listener_;
        const void *                 event_data_;

        TimeEvent
        ( int64_t timer_id,
          Clock::tick_type expire_time,
          Clock::tick_type interval,
          TimeEventListener *listener,
          const void *event_data
        ) : timer_id_(timer_id)
          , expire_time_(expire_time)
          , repeat_interval_(interval)
          , listener_(listener)
          , event_data_(event_data)
        {}
    };

    using TimeEventQueue = FixPooledLinkList<TimeEvent>;

    struct EventIdNode
    {
        int64_t                     timer_id_;
        TimeEventQueue::NodeType *  event_node_;

        EventIdNode(int64_t id, TimeEventQueue::NodeType * node)
          : timer_id_(id), event_node_(node)
        {}

        MAKE_POOLED_HASH_ENTRY(int64_t, timer_id_, size_t);
    };

    using EventIdNodeMap = PooledHash<EventIdNode>;

    
    void add
        ( int64_t timer_id,
          TimeEventListener *listener,
          const void *event_data,
          Clock::tick_type expire_time,
          Clock::tick_type interval
        );

    TimeEventQueue::NodeType* reposition (TimeEventQueue::NodeType* node);
    TimeEventQueue::NodeType* remove (TimeEventQueue::NodeType* node);

    TimeEventQueue              time_queue_;
    TimeEventQueue              pending_time_queue_;
    EventIdNodeMap              id_node_map_;
    std::atomic<int64_t>        timer_id_{0};
    std::mutex                  pending_mutex_;

};

}  // namespace alt