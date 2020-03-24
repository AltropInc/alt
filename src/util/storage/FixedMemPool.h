#pragma once

#include <util/sysinfo/Platform.h>
#include <util/numeric/Intrinsics.h>
#include <util/ipc/SharedMemory.h>
#include <util/sysinfo/Platform.h>
#include <util/sysinfo/SysConfig.h>     // for CACHE_LINE_ALIGN

#include <vector>
#include <atomic>


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
 * shrink. This is not thread safe.
 */
class FixedMemPool
{
  public:
      
    /// \brief Constructor
    /// \param value_size the size of the value contained in each slot. The slot
    /// size is aliged value size by 8 bytes.
    /// \param slot_num_per_slab number of slots per slab
    /// \param lazy_alloc when false, the first slab will be allocated when the
    /// pool is created; otherwise, slab will be allocated only as needed.
    FixedMemPool(size_t value_size,
            size_t slot_num_per_slab=100,
            bool lazy_alloc = true);

    /// \brief Destructor
    ~FixedMemPool();

    /// \brief allocate a slot in the pool
    /// \return the starting address of the slot
    void* alloc(uint16_t bin = 0) noexcept(false);

    /// \brief deallocate a slot in the pool
    /// \param p the starting address of the slot
    void free(void* p) noexcept(false);

    /// \brief get allocated bin of the slot
    /// \param p the starting address of the slot
    /// \note The bin is used as an id to indicate which fixed pool is used for the
    /// slot. This is used for the case when multiple pools are used and each pool
    /// is used for slots in a particular size. If always return 0 if a sigle pool
    /// is used. For using multiple pools, see PooledAllocator.
    static uint16_t getAlloactedBin(void* p);

  private:
    friend class PooledAllocator;

    struct EntryHeader;
    EntryHeader* newSlab()    noexcept(false);
    EntryHeader*              head_ { nullptr };
    size_t                    value_size_ { 0 };
    size_t                    slot_size_ { 0 };
    size_t                    slot_num_per_slab_ { 0 };
    std::vector<void*>        slab_list_;
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
  
    /*
    static FixedPool& instance()
    {
        static FixedPool s_allocator;
        return s_allocator;
    }
    */

    /// \brief Constructor
    FixedPool(): FixedMemPool(sizeof(T), growNum, lazyAlloc) {}

    /// \brief create an instance of T in the pool
    /// \tparam Args argument list for T constructor
    /// \return pointer to the instance
    template <typename... Args>
    T* acq(Args&&... args) noexcept(false)
    {
        void* p = FixedMemPool::alloc();
        return new (p) T (std::forward<Args>(args)...);
    }

    /// \brief delete an instance of T in the pool
    /// \tparam v pointer to the insatnce
    void del(T* v) noexcept(false)
    {
        v->~T();
        FixedMemPool::free(v);
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
    void* alloc() noexcept(false);

    /// \brief deallocate a slot in the pool
    /// \param p the starting address of the slot
    void free(void* p) noexcept(false);

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
        void* p = FixedMemPoolPrealloc::alloc();
        return new (p) T (std::forward<Args>(args)...);
    }

    /// \brief delete an instance of T in the pool
    /// \tparam v pointer to the insatnce
    void del(T* v) noexcept(false)
    {
        v->~T();
        FixedMemPoolPrealloc::free(v);
    }
};

using SharedFixedMemPool = SharedContainer<SharedMemory, FixedMemPoolPrealloc>;
template<typename T> using SharedFixedPool
    = SharedContainer<SharedMemory, FixedPoolPrealloc<T>>;

} // name space alt
