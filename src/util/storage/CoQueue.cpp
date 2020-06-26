#include "CoQueue.h"            // for CoQueue, this class

namespace alt
{

CoQueueBase::CoQueueBase(bool multiple_writers)
    : multiple_writers_(multiple_writers)
    , head_(&empty_node_)
    , tail_(&empty_node_)
    , last_consumed_(&empty_node_)
{
}

CoQueueBase::~CoQueueBase()
{
    release(std::numeric_limits<int>::max());
}

void CoQueueBase::enqueue(EntryBase* node)
{
    // writers are responsible to to release nodes consumed. Try to free two notes
    // before enqueue
    release(2);

    if (multiple_writers_)
    {
        // If we have multiple writers, tail_->next_ and tail must be updated
        // together
        std::scoped_lock lock(writers_mutex_);
        tail_.load(std::memory_order_relaxed)->next_ = node;
        tail_.store(node, std::memory_order_release);
    }
    else
    {
        tail_.load(std::memory_order_relaxed)->next_ = node;
        tail_.store(node, std::memory_order_release);
    }

    if (blocking_mode_used_.load(std::memory_order_acquire))
    {
        cv_.notify_all();
    }
}

CoQueueBase::EntryBase* CoQueueBase::dequeue()
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

CoQueueBase::EntryBase* CoQueueBase::blockingDequeue()
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

void CoQueueBase::commit(EntryBase* entry)
{
    entry->consumed_.store(true, std::memory_order_release);
}

void CoQueueBase::release(int trim_num)
{
    while (trim_num > 0)
    {
        auto n = empty_node_.next_.load(std::memory_order_relaxed);
        if (!n || !n->consumed_.load(std::memory_order_acquire)) return;

        auto next = n->next_.load(std::memory_order_relaxed);
        if (empty_node_.next_.compare_exchange_strong(n, next,
                std::memory_order_acq_rel, std::memory_order_relaxed))
        {
            // n is successfully detached here, delete it 
            del(n);
            --trim_num;
        }
        // someone else detached empty_node_.next_. Continue to try to release
        // more until trim_num satisfied
    }
}

template<> Allocator& CoQueue<Allocator>::allocator_ = Allocator::instance();
template<> PooledAllocator& CoQueue<PooledAllocator>::allocator_ = PooledAllocator::instance();

} // namespace alt