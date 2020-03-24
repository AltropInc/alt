#pragma once

#include "LinkedList.h"
#include <util/sysinfo/Platform.h>
#include <vector>
#include <functional>
#include <atomic>
#include <memory>
#include  <condition_variable>  // For condition_variable

namespace alt {

/**
 * \class QueueBase
 * \brief implements a Single-Producer/Single-Consumer queue
 * @note  Dequeue is wait-free when blocking_ is false. Enqueue operation is always wait-free.
 * Elements in the queue are chained in a linklist which can be allocated a memory pool.
 * Derived classes from QueueBase are:
 * QueueT<T, Allocator> is for homogeneous entries of type T
 * QueueT<T, FixedPool<T>> is for homogeneous entries of type T allocated in fixed pool
 * Queue<Allocator> is for heterogeneous entries of different types
 * Queue<PooledAllocator> is for heterogeneous entries of different types in memory pool
 */
class QueueBase
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
    QueueBase();

    /// \brief Destructor
    /// \note the producer and consumer must have stopped using the queue before the
    /// destructor can be safely called.
    ~QueueBase();

    /// \brief enqueue called by the producer thread
    /// \param node  pointer to the entry to be pushed into the queue
    void enqueue(EntryBase* node);

    /// \brief commit read.
    /// \return pointer to the first unprocessed entry if any, otherwise nullptr
    static void commit(EntryBase* entry);

    /// \brief detach the first unprocessed entry, called by the consumer thread
    /// \return pointer to the first unprocessed entry or nullptr if blocking_ is fase and
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

    // mutex and conditional variable, only used when blocking_ is true
    std::mutex                   mutex_;
    std::condition_variable      cv_;

    // when blocking_mode_used_ is true, blocking dequeue is called. We will need to notify
    // consumers after produce
    std::atomic<bool>            blocking_mode_used_ { false };
    
    // accessed by producer
    EntryBase                        empty_node_;
    const EntryBase*                 head_;

    alignas(64) std::atomic<EntryBase*>   tail_;

    // TODO: use cpu info cache_line_size for alignment
    // written by consumer, read by producer
    alignas(64) std::atomic<EntryBase*>   last_consumed_;
};

/**
 * \class QueueT
 * \brief implements a Single-Producer/Single-Consumer queue with entries of the same type of T
 */
template <class T, class Alloc = Allocator>
class QueueT: public QueueBase
{
  public:
    //using AllocType = FixedPool<T>;

    QueueT(Alloc& allocator): allocator_(allocator) {};

    template <typename... Args>
    T* acquire(Args&&... args)
    {
        return tf_pnew(allocator_, T, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void enqueue(Args&&... args)
    {
        T* node = tf_pnew(allocator_, T, std::forward<Args>(args)...);
        enqueue(node);
    }

  protected:
    void del(EntryBase* n) override {  tf_pdel(allocator_, T, reinterpret_cast<T*>(n)); }
    static_assert(std::is_base_of<QueueBase::EntryBase, T>::value);

    Alloc &    allocator_;
};

/**
 * \class PooledQueue
 * \brief implements a Single-Producer/Single-Consumer queue with entries of the same type of T
 * and allocated in fixed pool of T
 */
template <class T> using PooledQueueT = QueueT<T, FixedPool<T>>;

/**
 * \class Queue
 * \brief implements a Single-Producer/Single-Consumer queue with entries of different types
 */
template <class Alloc = Allocator>
class Queue: public QueueBase
{
  public:

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QueueBase::EntryBase, T>::value>* = nullptr,
              typename... Args>
    T* acquire(Args&&... args)
    {
        void *p = Alloc::alloc(sizeof(T));
        return p ? new(p)T(std::forward<Args>(args)...) : (T*)nullptr;
        //return tf_pnew(allocator_, T, std::forward<Args>(args)...);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QueueBase::EntryBase, T>::value>* = nullptr,
              typename... Args>
    void enqueue(Args&&... args)
    {
        void *p = Alloc::allocate(sizeof(T));
        if (p)
        {
            QueueBase::enqueue(new(p)T(std::forward<Args>(args)...));
        }
        //T* node = tf_pnew(allocator_, T, std::forward<Args>(args)...);
        //enqueue(node);
    }

  protected:
    void del(EntryBase* n) override
    {
        if (n)
        {
            n->~EntryBase();
            Alloc::deallocate(static_cast<void*>(n));
        }
        //tf_pdel(allocator_, EntryBase, n);
    }
};

/**
 * \class PooledQueue
 * \brief implements a Single-Producer/Single-Consumer queue with entries of different types
 * allocated in a set of fixed pools
 */
using PooledQueue = Queue<PooledAllocator>;

}