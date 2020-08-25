
#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file Reactor.h
 * @library alt_util
 * @brief Implements an event/message dispatch mechanism per thread for events delivered
 * concurrently from multiple sources. The mechanism delivers the incoming events/nessages
 * synchronously to its associated event handlers or message listeners. A reactor handles
 * three types of events:
 *    - Timer events. One timer queue per reactor.
 *    - event messages posted for other threads through concurrent queue or from other
 *      processes through shared memory queue. These are non-bloking messages queues and
 *      a reactor may have multiple queues.
 *    - events/messages polled from event poller in network or message queue from underlying
 *      operating system. A reactor can have only one queue for this purpose. Because this
 *      event poller typically blocks caller, though with a timeout, it is not practical
 *      to have more than one event pollers to poll events with a timeout one after
 *      another. It is better to put all event sources in one queue and pull them all at
 *      once. If operating system does not support one poll for all events, you will need
 *      to poll these in separate threads. For instance, in Windows system, you cannot
 *      poll GUI messages and network events in one thread. A practical solution is to
 *      have a separate thread to handle network events.
 */

#include <util/Defs.h>                  // for ALT_UTIL_PUBLIC
#include "TimerQueue.h"                 // for TimerQueue
#include "MsgPoller.h"                  // for MsgPoller
#include "EventPoller.h"                // for EventPoller
#include <util/types/Clock.h>           // for Clock
#include <util/storage/CoQueue.h>       // for CoQueue

#include <functional>                   // for function
#include <vector>                       // for vector
#include <memory>                       // for unique_ptr
#include <atomic>                       // for atomic

namespace alt {

//$ g++ xl01.cpp -o xl01 -I/usr/X11R6/include -I/usr/X11R6/include/X11 -L/usr/X11R6/lib 
//-L/usr/X11R6/lib/X11 -lX11

class ReactorOwner
{
  public:
    virtual void onReactorStop() = 0;
};

/**
 * \class Reactor
 * \ingroup Util
 * \brief  Implements an event/message dispatcher
 */
class ALT_UTIL_PUBLIC Reactor
{
 public:

    /// \brief constructs a reactor
    /// \param owner the object that listens to the events of the reactor itself - events
    /// such as on reactor start and stop. It can be a null pointer
    Reactor(ReactorOwner* owner=nullptr);
 
    /// \brief start and run reactor loop
    /// \param poll_interval a period to rest between loop iterations. This is also used
    /// to determine the poll timeout in event poller and the resultion of the timer events.
    /// The poll timeout could be longer if this is not a busy poller and there is no non-
    /// blocking messages queues and no timer events within the poll_interval in future. 
    /// Because of the timeout setting in blocking event poller, the timer event cannot
    /// have a highier resolution than poll_interval.
    /// \param busy_poller when true, the reactor runs in busy polling mode, typically used
    /// by low-latency threads running on dedicated cpu core, event or message poller will
    /// typically run in non-block mode with poll_interval (timeout) set to zero. However,
    /// in busy polling mode, you can still set a non-zero poll timeout in a condition that
    /// does not affect timer events and other non-bloking messages queues, this may help
    /// in reducing the heat by a heavy cpu load.
    /// \param power_saving when true, the reactor runs in power saving mode, typically for
    /// mobile devices where battery life is impportant. In power saving mode, the waiting
    /// timeout for block polling can be prolonged with a more relaxed condition that
    /// allows the presence of non-blocking queues for messages from other threads or
    /// processes.
    /// \param max_poll_timeout the maximum poll timeout for block polling
    void run(
        Clock::tick_type poll_interval=Clock::one_millisec,
        bool busy_poller=false,
        bool power_saving=false,
        Clock::tick_type max_poll_timeout=Clock::one_sec);

    /// \brief stop reactor loop
    void stop();

    /// \brief tells if the reactor is stopped
    bool stopoed() const;

    /// \brief tells if the reactor is running
    bool running() const;

    /// \brief gets the timer queues. A timer queue will be created if it
    /// does not exist. 
    TimerQueue & getTimerQueue();
    
    /// \brief sets the event poller 
    void setEventPoller(EventPoller* event_poller);

    /// \brief adds a message poller
    /// \return the id of the message poller
    int addMessagePoller(MessagePoller* msg_poller);

    /// \brief gets a message poller
    /// \param id the id of the message poller
    /// \return pointer to the message poller
    MessagePoller * getMessagePoller(int id);

    /// \brief creates a thread message poller using a CoQueue. A new thread message poller
    /// will be created only if it does not exist.
    /// \tparam CoQueueType the type of CoQueue, see CoQueue
    /// \param handler the message handler
    /// \param max_poll_num maximum number of messages to be polled each time. Set to a small
    /// number to prevent starvation to other queue pollers, if there is any.
    /// \param use_pool true if use pooled allocator otherwise use malloc for messages
    void createThreadMsgPoller (CoQueueMsgHandler& handler, int max_poll_num=5, bool use_pool=false);

    /// \brief post a thread message
    /// \tparam CoQueueType the type of CoQueue, see CoQueue
    /// \param handler the message handler
    /// \param use_pool true if use pooled allocator otherwise use malloc for messages
    template <class MsgT, class... Args>
    void notify(Args&&... args)
    {
        auto poller = getMessagePoller(thread_msg_poller_);
        if (poller)
        {
            if (thread_msg_poller_use_pool_)
            {
                reinterpret_cast<CoQueueMsgPoller<Allocator>*>(poller)->
                    notify<MsgT>(std::forward<Args>(args)...);               
            }
            else
            {
                reinterpret_cast<CoQueueMsgPoller<PooledAllocator>*>(poller)->
                    notify<MsgT>(std::forward<Args>(args)...);
            }
        }
    }

    /// \brief tells if the reactor is running in busy polling mode
    bool isBusyPoller() const { return busy_poller_; }

    /// \brief tells if the reactor is running in power saving mode
    bool isPowerSaving() const { return power_saving_; }

    /// \brief gets poll interval value
    Clock::tick_type pollInterval() const { return poll_interval_; }

    /// \brief gets maximum poll timeout value
    Clock::tick_type maxPollTimout() const { return max_poll_timeout_; }

  private:
    ReactorOwner*                                   owner_;
    std::unique_ptr<TimerQueue>                     timer_queue_;
    std::vector<std::unique_ptr<MessagePoller>>     message_pollers_;
    std::unique_ptr<EventPoller>                    event_poller_;
    int                                             thread_msg_poller_ {-1};
    bool                                            thread_msg_poller_use_pool_{false};

    std::atomic<bool>     running_{false};
    std::atomic<bool>     stopped_{false};
    std::atomic<bool>     stop_flag_{false};
    Clock::tick_type      poll_interval_{Clock::one_millisec};
    bool                  busy_poller_ {false};
    bool                  power_saving_ {false};
    Clock::tick_type      max_poll_timeout_{Clock::one_sec};
};

}  // namespace alt