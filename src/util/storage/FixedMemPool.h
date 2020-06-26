#pragma once

#include <util/system/Platform.h>
#include <util/Defs.h>                 // for ALT_UTIL_PUBLIC
#include <util/numeric/Intrinsics.h>
#include <util/ipc/SharedMemory.h>
#include <util/system/SysConfig.h>     // for CACHE_LINE_ALIGN

#include <vector>
#include <atomic>
#include <mutex>                       // for mutex


namespace alt
{

class PooledAllocator;

//-----------------------------------------------------------------------------
// Fixed memory pool
//-----------------------------------------------------------------------------
/**
 * \struct FixedMemPool
 * \ingroup ContainerUtils
 * \brief An growable memory pool with fix-sized slots. The pool is expanded
 * by slab which hold a number of slots. Once expanded, the pool will not
 * shrink.
 * \note thread-safe and non-thread-safe versions should not be mixed in use
 */
class FixedMemPool
{
  public:
      
    /// \brief Constructor. This can only be safely done when all other threads using
    /// this pool have not started accessing to this.
    /// \param value_size the size of the value contained in each slot. The slot
    /// size is aliged value size by 8 bytes.
    /// \param slot_num_per_slab number of slots per slab
    /// \param lazy_alloc when false, the first slab will be allocated when the
    /// pool is created; otherwise, slab will be allocated only as needed.
    FixedMemPool(size_t value_size,
            size_t slot_num_per_slab=100,
            bool lazy_alloc = true);

    /// \brief Destructor. This can only be safely done when all other threads using
    /// this pool no longer have access to this.
    /// \note For performance reason, FixedMemPool does not track allocated memory
    /// internally, therefore, the user is responsible to free all allocated entries
    /// (if entries stored in the pool is not trivially destructable) before this
    /// instance is destroyed.
    ~FixedMemPool();

    /// \brief allocate a slot in the pool
    /// \return the starting address of the slot
    void* allocate(uint16_t bin = 0) noexcept(false);

    /// \brief deallocate a slot in the pool
    /// \param p the starting address of the slot
    void deallocate(void* p) noexcept(false);

    /// \brief allocate a slot in the pool (thread safe version)
    /// \return the starting address of the slot
    void* coAllocate(uint16_t bin = 0) noexcept(false);

    /// \brief deallocate a slot in the pool (thread safe version)
    /// \param p the starting address of the slot
    void coDeallocate(void* p) noexcept(false);

    /// \brief deallocate all slots in the pool. This can only be used in non-thread-safe
    /// version
    /// \note FixedMemPool does not track allocated memory internally, therefore,
    /// the user is responsible to free all allocated entries before this is called
    void clear() noexcept(false);

    /// \brief get allocated bin of the slot
    /// \param p the starting address of the slot
    /// \note The bin is used as an id to indicate which fixed pool is used for the
    /// slot. This is used for the case when multiple pools are used and each pool
    /// is used for slots in a particular size. It always return 0 if a single pool
    /// is used. For using multiple pools, see PooledAllocator in Allocator.h
    static uint16_t getAlloactedBin(void* p);

    /// \brief allocate a memory block that is bigger than the slot in the pool.
    /// \param size the size of space to be allocated
    /// \param bin the number set in the allocated header. Use getAlloactedBin to
    /// check the address, if the bin number is equal to the bin given here, it is
    /// allocated by allocateBigSize and must be freed by deallocateBigSize
    /// \return the starting address of the allocated memory
    /// \note this is allocated in heap directly, not in the pool.
    static void* allocateBigSize(size_t size, uint16_t bin);

    /// \brief deallocate a memory block allocated by allocateBigSize
    static void deallocateBigSize(void* p);

  private:
    friend class PooledAllocator;

    void initSlab(void* slab);

    struct EntryHeader;
    EntryHeader* newSlab()    noexcept(false);
    EntryHeader*              head_ { nullptr };
    size_t                    value_size_ { 0 };
    size_t                    slot_size_ { 0 };
    size_t                    slot_num_per_slab_ { 0 };
    std::vector<void*>        slab_list_;
    std::mutex                mutex_;
};

//-----------------------------------------------------------------------------
// Fixed memory pool template with a single type
//-----------------------------------------------------------------------------
/**
 * \ingroup ContainerUtils
 * \brief An template for expandable memory pool for a given type
 * \tparam T the type of the entry in each slot
 * \tparam growNum the number of entries for the capacity growing
 * \tparam lazyAlloc when false, the initial chunk of the pool will be allocated
 */
template <typename  T, size_t growNum = 100, bool lazyAlloc = true>
class FixedPool: public FixedMemPool
{
  public:

    /// \brief Constructor
    FixedPool(): FixedMemPool(sizeof(T), growNum, lazyAlloc) {}

    /// \brief create an instance of T in the pool
    /// \tparam Args argument list for T constructor
    /// \return pointer to the instance
    template <typename... Args>
    T* acq(Args&&... args) noexcept(false)
    {
        void* p = FixedMemPool::allocate();
        return new (p) T (std::forward<Args>(args)...);
    }

    /// \brief delete an instance of T in the pool
    /// \tparam v pointer to the instance
    void del(T* v) noexcept(false)
    {
        v->~T();
        FixedMemPool::deallocate(v);
    }
    
    /// \brief create an instance of T in the pool (thread safe version)
    /// \tparam Args argument list for T constructor
    /// \return pointer to the instance
    template <typename... Args>
    T* coAcq(Args&&... args) noexcept(false)
    {
        void* p = FixedMemPool::allocate();
        return new (p) T (std::forward<Args>(args)...);
    }

    /// \brief delete an instance of T in the pool (thread safe version)
    /// \tparam v pointer to the instance
    void coDel(T* v) noexcept(false)
    {
        v->~T();
        FixedMemPool::deallocate(v);
    }
};


//-----------------------------------------------------------------------------
// Fixed memory pool
//-----------------------------------------------------------------------------
/**
 * \struct FixedMemPoolPrealloc
 * \ingroup ContainerUtils
 * \brief A preallocated memory pool with fix-sized slots. When the pool is full,
 * allocation will fail. This is used for the case where maximum number of slots are
 * known or the case where the memory cannot be expanded (e.g. in a shared memory).
 * This is not thread safe.
 */
// TODO: justify usage of this and then decide whether to keep it or remove it
class FixedMemPoolPrealloc
{
  public:
      
    /// \brief Constructor
    /// \param addr  address for the preallocated memory pool
    /// \param slot_num slot number in the preallocated memory
    /// \note Use this constructor to pass the memory allocated outside
    FixedMemPoolPrealloc(char* addr, size_t value_size, size_t slot_num);

    /// \brief Constructor
    /// \param slot_num slot number in the preallocated memory
    /// \note Use this constructor to allocate the memory internally
    FixedMemPoolPrealloc(size_t value_size, size_t slot_num);

    /// \brief Empty constructor. setAddr must be called to set the memory address
    FixedMemPoolPrealloc();

    /// \brief Destructor
    ~FixedMemPoolPrealloc();

    /// \brief Get memory pool header size
    static size_t requiredHeaderSize();

    /// \brief Get required memory size for both the header and the memory pool
    /// \param capacity the size of the memory pool
    /// \note This function is required by SharedMemory template
    static size_t requiredSize(size_t value_size, size_t slot_num);

    /// \brief Set the preallocated memory pool
    /// \param addr  address for the preallocated memory pool
    /// \param pool_sz  size of the preallocated memory
    /// \note This must be called if empty constructor is used
    void setAddr(char* addr, size_t value_size, size_t slot_num);

    /// \brief allocate a slot in the pool
    /// \return the starting address of the slot
    void* allocate() noexcept(false);

    /// \brief deallocate a slot in the pool
    /// \param p the starting address of the slot
    void deallocate(void* p) noexcept(false);

  private:

    struct EntryHeader;
    struct PoolHeader
    {
        const bool              owns_buffer_  {false};
        size_t                  value_size_   { 0 };
        size_t                  slot_size_    { 0 };
        size_t    			    slot_num_     {0};
        char*       			addr_         {nullptr};

        //EntryHeader*            head_ {nullptr};
        CACHE_LINE_ALIGN std::atomic<EntryHeader*>  head_ {nullptr};

        PoolHeader(char* addr, size_t slot_num, size_t value_size);
        PoolHeader(size_t slot_num_, size_t value_size_);
        PoolHeader() = default;
        void initialize();
    }
    header_;
};


//-----------------------------------------------------------------------------
// Fixed memory pool template with a single type
//-----------------------------------------------------------------------------
/**
 * \ingroup ContainerUtils
 * \brief An template for preallocated pool for a given type
 * \tparam T the type of the entry in each slot
 * \tparam SlotNum the number of entries preallocated
 */
template <typename  T>
class FixedPoolPrealloc: public FixedMemPoolPrealloc
{
  public:

    /// \brief Constructor
    FixedPoolPrealloc(): FixedMemPoolPrealloc() {}
    FixedPoolPrealloc(size_t slot_num): FixedMemPoolPrealloc(sizeof(T), slot_num) {}
    FixedPoolPrealloc(char* addr, size_t slot_num): FixedMemPoolPrealloc(addr, sizeof(T), slot_num) {}

    void setAddr(char* addr, size_t slot_num)
    {
        FixedMemPoolPrealloc::setAddr(addr, sizeof(T), slot_num);
    }

    static size_t requiredSize(size_t slot_num)
    {
        return FixedMemPoolPrealloc::requiredSize(sizeof(T), slot_num);
    }

    /// \brief create an instance of T in the pool
    /// \tparam Args argument list for T constructor
    /// \return pointer to the instance
    template <typename... Args>
    T* acq(Args&&... args) noexcept(false)
    {
        void* p = FixedMemPoolPrealloc::allocate();
        return new (p) T (std::forward<Args>(args)...);
    }

    /// \brief delete an instance of T in the pool
    /// \tparam v pointer to the instance
    void del(T* v) noexcept(false)
    {
        v->~T();
        FixedMemPoolPrealloc::deallocate(v);
    }
};

using SharedFixedMemPool = SharedContainer<SharedMemory, FixedMemPoolPrealloc>;
template<typename T> using SharedFixedPool
    = SharedContainer<SharedMemory, FixedPoolPrealloc<T>>;

} // name space alt
