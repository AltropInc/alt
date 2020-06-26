#pragma once

#include "LinkedList.h"
#include <util/system/Platform.h>
#include <vector>
#include <functional>
#include <atomic>                    // for atomic
#include <mutex>                     // for mutex
#include  <condition_variable>       // for condition_variable


namespace alt {

/**
 * \class CoQueueBase
 * \brief implements thread safe concurrent queue. Lock free for single writer mode
 * @note  Dequeue is wait-free if blocking_ is false. Enqueue operation is always wait-free.
 * Elements in the queue are chained in a linklist which can be allocated a memory pool.
 * Derived classes from CoQueueBase are:
 * CoQueueT<T, Allocator> is for homogeneous entries of type T
 * CoQueueT<T, FixedPool<T>> is for homogeneous entries of type T allocated in fixed pool
 * Queue<Allocator> is for heterogeneous entries of different types
 * Queue<PooledAllocator> is for heterogeneous entries of different types in memory pool
 */
class CoQueueBase
{
  public:

    /**
     * \struct EntryBase
     * \brief Base type for all element types of queue entry.
     */
    struct EntryBase
    {
        std::atomic<EntryBase*> next_ {nullptr};
        std::atomic<bool>   consumed_ {false};

        virtual ~EntryBase() {}
    };

    /// \brief Constructor
    /// unprocessed entries
    CoQueueBase(bool multiple_writers =false);

    /// \brief Destructor
    /// \note the producer and consumer must have stopped using the queue before the
    /// destructor can be safely called.
    ~CoQueueBase();

    /// \brief enqueue called by the producer thread
    /// \param node  pointer to the entry to be pushed into the queue
    void enqueue(EntryBase* node);

    /// \brief commit read.
    /// \return pointer to the first unprocessed entry if any, otherwise nullptr
    static void commit(EntryBase* entry);

    /// \brief detach the first unprocessed entry, called by the consumer thread
    /// \return pointer to the first unprocessed entry or nullptr if blocking_ is false and
    /// there is no unprocessed entry
    /// \note The caller should not delete the returned entry. The queue will remove
    /// processed entry internally.
    EntryBase* dequeue();
    EntryBase* blockingDequeue();

  protected:

    /// \brief delete the entry
    /// \param n pointer to the entry to be deleted.
    /// \note this function needs to be overriden in derived class when different allocator
    /// is used
    virtual void del(EntryBase* n) { delete n; }

    /// \brief release processed entries
    /// \param trim_num maximum number of processed entries can be released
    /// \note this should be only called by the producer thread
    void release(int trim_num);

    // when blocking_mode_used_ is true, blocking dequeue is called. We will need to notify
    // consumers after enqueue
    std::atomic<bool>            blocking_mode_used_ { false };
    // mutex and conditional variable, only used when blocking_ is true
    std::mutex                   mutex_;
    std::condition_variable      cv_;
    
    const bool                   multiple_writers_ { false };
    // mutex used only when multiple_writers_ is true
    std::mutex                   writers_mutex_;
    
    // accessed by writers
    EntryBase                        empty_node_;
    const EntryBase*                 head_;

    alignas(64) std::atomic<EntryBase*>   tail_;

    // TODO: use cpu info cache_line_size for alignment
    // written by readers, read by writers
    alignas(64) std::atomic<EntryBase*>   last_consumed_;
};

/**
 * \class CoQueueT
 * \brief implements a concurrent queue with entries of the same type of T
 */
template <class T, class Alloc = Allocator>
class CoQueueT: public CoQueueBase
{
  public:

    CoQueueT(Alloc& allocator): allocator_(allocator) {};

    template <typename... Args>
    T* acquire(Args&&... args)
    {
        return alt_pnew(allocator_, T, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void enqueue(Args&&... args)
    {
        T* node = alt_pnew(allocator_, T, std::forward<Args>(args)...);
        enqueue(node);
    }

  protected:
    void del(EntryBase* n) override {  alt_pdel(allocator_, T, reinterpret_cast<T*>(n)); }
    static_assert(std::is_base_of<CoQueueBase::EntryBase, T>::value);

    Alloc &    allocator_;
};

/**
 * \class PooledQueueT
 * \brief implements a concurrent queue with entries of the same type of T
 * and allocated in fixed pool of T
 */
template <class T> using PooledCoQueueT = CoQueueT<T, FixedPool<T>>;

/**
 * \class Queue
 * \brief implements a concurrent queue with entries of different types
 */
template <class Alloc = Allocator>
class CoQueue: public CoQueueBase
{
  public:

    template <typename T,
              typename std::enable_if_t<std::is_base_of<CoQueueBase::EntryBase, T>::value>* = nullptr,
              typename... Args>
    T* acquire(Args&&... args)
    {
        return alt_pnew(allocator_, T, std::forward<Args>(args)...);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<CoQueueBase::EntryBase, T>::value>* = nullptr,
              typename... Args>
    void enqueue(Args&&... args)
    {

        T* node = alt_pnew(allocator_, T, std::forward<Args>(args)...);
        CoQueueBase::enqueue(node);
    }

  protected:
    void del(EntryBase* n) override
    {
        alt_pdel(allocator_, EntryBase, n);
    }

    static Alloc& allocator_;
};

/**
 * \class PooledCoQueue
 * \brief implements a concurrent queue with entries of different types
 * allocated in a set of fixed pools
 */
using PooledCoQueue = CoQueue<PooledAllocator>;

}
