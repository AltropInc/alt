#include "Queue.h"

namespace alt
{

class ConcurrentQueue
{
  public:

    // brief Base type for all element types of queue entry. 
    struct EntryBase
    {
        std::atomic<EntryBase*> next_ {nullptr};
        std::atomic<bool>   consumed_ {false};
        virtual ~EntryBase() {}
    };

    ConcurrentQueue()
        : head_(&empty_node_)
        , tail_(&empty_node_)
        , last_consumed_(&empty_node_)
    {
    }

    // the producer and consumer must have stopped using the queue before the
    // destructor can be safely called.
    ~ConcurrentQueue();

    void Push(EntryBase* node)
    {
        release(2);

        // update tail_'s next. If the next_ pointer of tail_to_update is not nullptr,
        // someone else has updated the tail_, so I need to update the tail again
        EntryBase * expected_tail_next;
        EntryBase * tail_to_update;
        do
        {
            tail_to_update = tail_;
            expected_tail_next = nullptr;
        }
        while (!tail_to_update->next_.compare_exchange_weak(
                    expected_tail_next, node,
                    std::memory_order_release, std::memory_order_acquire)
            );

        // update the tail_ itself. If tail_ is not tail_to_update in which I updated the next pointer,
        // someone else has updated the tail_ after I updated tail_'s next and then I do nothing.
        tail_.compare_exchange_strong(tail_to_update, node);
        cv_.notify_all();      
    }

    EntryBase* TryPop()
    {
        EntryBase * last_consumed;
        EntryBase * next_last_consumed;
        do
        {
            last_consumed = last_consumed_.load(std::memory_order_relaxed);
            next_last_consumed = last_consumed->next_;
            if (next_last_consumed == nullptr)
            {
                return nullptr;
            }
        }
        while (!last_consumed_.compare_exchange_weak(
                    last_consumed, next_last_consumed,
                    std::memory_order_release, std::memory_order_acquire)
            );

        return next_last_consumed;  
    }

    // 
    EntryBase* Pop()
    {
        auto entry = TryPop();
        if (!entry)
        {
            blocking_mode_used_.store(true, std::memory_order_release);
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this, &entry] { entry = TryPop(); return entry != nullptr; });
        }
        return entry;
    }

    static void Release(EntryBase* entry)
    {
        entry->consumed_.store(true, std::memory_order_release);
    }

  protected:

    // override this if Entry node is allocated in different memory protocol
    virtual void del(EntryBase* n) { delete n; }

    void release(int trim_num)
    {
        while (trim_num > 0)
        {
            auto n = empty_node_.next_.load(std::memory_order_relaxed);
            if (!n || !n->consumed_.load(std::memory_order_acquire)) return;

            auto next = n->next_.load(std::memory_order_relaxed);
            if (empty_node_.next_.compare_exchange_strong(n, next))
            {
                del(n);
                --trim_num;
            }

            // someone else changed empty_node_.next and continue to try to release
        }
    }

    // mutex and conditional variable, only used when blocking_ is true
    std::mutex                   mutex_;
    std::condition_variable      cv_;

    // when blocking_mode_used_ is true, blocking dequeue is called. We will need to notify
    // consumers after produce
    std::atomic<bool>            blocking_mode_used_ { false };
    
    // accessed by producer
    EntryBase                        empty_node_;
    const EntryBase*                 head_;

    // Assuming cache line size is 64
    alignas(64) std::atomic<EntryBase*>   tail_;
    alignas(64) std::atomic<EntryBase*>   last_consumed_;
};

QueueBase::QueueBase()
    : head_(&empty_node_)
    , tail_(&empty_node_)
    , last_consumed_(&empty_node_)
{
}

void QueueBase::enqueue(EntryBase* node)
{
    release(2);

    // update tail_'s next. If the next_ pointer of tail_to_update is not nullptr,
    // someone else has updated the tail_, so I need to update the tail again
    EntryBase * expected_tail_next;
    EntryBase * tail_to_update;
    do
    {
        tail_to_update = tail_;
        expected_tail_next = nullptr;
    }
    while (!tail_to_update->next_.compare_exchange_weak(
                expected_tail_next, node,
                std::memory_order_release, std::memory_order_acquire)
          );

    // update the tail_ itself. If tail_ is not tail_to_update in which I updated the next pointer,
    // someone else has updated the tail_ after I updated tail_'s next and then I do nothing.
    tail_.compare_exchange_strong(tail_to_update, node);

    if (blocking_mode_used_.load(std::memory_order_acquire))
    {
        cv_.notify_all();
    }
}

QueueBase::EntryBase* QueueBase::dequeue()
{
    EntryBase * last_consumed;
    EntryBase * next_last_consumed;
    do
    {
        last_consumed = last_consumed_.load(std::memory_order_relaxed);
        next_last_consumed = last_consumed->next_;
        if (next_last_consumed == nullptr)
        {
            return nullptr;
        }
    }
    while (!last_consumed_.compare_exchange_weak(
                last_consumed, next_last_consumed,
                std::memory_order_release, std::memory_order_acquire)
          );

    return next_last_consumed;
}

QueueBase::EntryBase* QueueBase::blockingDequeue()
{
    auto entry = dequeue();
    if (!entry)
    {
        blocking_mode_used_.store(true, std::memory_order_release);
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this, &entry] { entry = dequeue(); return entry != nullptr; });
    }
    return entry;
}

void QueueBase::commit(EntryBase* entry)
{
    entry->consumed_.store(true, std::memory_order_release);
}

void QueueBase::release(int trim_num)
{
    while (trim_num > 0)
    {
        auto n = empty_node_.next_.load(std::memory_order_relaxed);
        if (!n || !n->consumed_.load(std::memory_order_acquire)) return;

        auto next = n->next_.load(std::memory_order_relaxed);
        if (empty_node_.next_.compare_exchange_strong(n, next,
                std::memory_order_acq_rel, std::memory_order_relaxed))
        {
            del(n);
            --trim_num;
        }
        // someone else changed empty_node_.next and continue to try to release
    }
}

QueueBase::~QueueBase()
{
    release(std::numeric_limits<int>::max());
}

} // namespace alt