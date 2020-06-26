
#pragma once

#include <util/storage/CoQueue.h>       // for CoQueue
#include <util/types/Clock.h>           // for Clock::duration
#include <util/types/TemplateHelper.h>  // for NONCOPYABLE

namespace alt {

/**
 * \class MessagePoller
 * \ingroup Util
 * \brief Abstract Message Poller Interface
 */
class MessagePoller
{
  public:
    virtual void poll(Clock::tick_type tick_realtime) =0;
};

/**
 * \class CoQueueMsg
 * \ingroup Util
 * \brief A base class for message poller using concrent queue
 */
struct CoQueueMsg: CoQueueBase::EntryBase
{
    uint32_t msg_type_;

    CoQueueMsg(uint32_t msg_type) :  msg_type_(msg_type) {} 
};

/**
 * \class CoQueueMsgHandler
 * \ingroup Util
 * \brief Abstract CoQueue Msg Handler interface
 */
class CoQueueMsgHandler
{
  public:
    virtual void processMessage(Clock::tick_type tick_realtime, const CoQueueMsg* msg) =0;
};

/**
 * \class CoQueueMsgPoller
 * \ingroup Util
 * \brief Message Poller use concrent queue
 */
template <class Alloc>
class CoQueueMsgPoller: private MessagePoller
{
  public:

    using CoQueueType = CoQueue<Alloc>;

    NONCOPYABLE(CoQueueMsgPoller);

    CoQueueMsgPoller(CoQueueMsgHandler& msg_handler, int max_poll_num=5)
        : msg_handler_(msg_handler)
    {}

    template <class MsgT,
             typename std::enable_if_t<std::is_base_of<CoQueueMsg, MsgT>::value>* = nullptr,
             class... Args>
    void notify(Args&&... args)
    {
        co_queue_.template enqueue<MsgT>(std::forward<Args>(args)...);
    }

  private:
  
    friend class Reactor;

    void poll(Clock::tick_type tick_realtime) override
    {
        CoQueueBase::EntryBase* entry = co_queue_.dequeue();
        for (int i=0; i< max_poll_num_ && entry; ++i)
        {
            msg_handler_.processMessage(tick_realtime, reinterpret_cast<CoQueueMsg*>(entry));
            entry->consumed_.store(true, std::memory_order_release);
            entry = co_queue_.dequeue();
        }
    }

    CoQueueType           co_queue_;
    CoQueueMsgHandler&    msg_handler_;
    int                   max_poll_num_;
};

}

