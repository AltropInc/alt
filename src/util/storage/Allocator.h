#pragma once

#include "FixedMemPool.h"  // for FixedMemPool
#include <stdlib.h>        // for malloc/free
#include <iostream>
#include <typeinfo> 
#define MEM_POOL_DEBUG      1

namespace alt
{
//-----------------------------------------------------------------------------
// Memory tracker for debug
//-----------------------------------------------------------------------------
#if defined MEM_POOL_DEBUG
class MemTracker
{
    struct MemTrackerImpl;
    MemTrackerImpl      * impl_;
  public:
    uint64_t getTotalCount() const;
    size_t reportMostUsed(char* buffer, size_t buffer_sz) const;
    void reportMostUsed() const;
    void untrack(void * p);
    void track(void * p, const char *file, int line);
    MemTracker();
    ~MemTracker();
};
#endif

/**
 * \class FixedMemPoolBin
 * \brief a set of FixedMemPool arranged in different slot sizes. Allocations
 * happen in a selected FixedMemPool (bin) based on the required size. This is
 * used for allocating heterogeneous containers in fixed pools. This uses a shared
 * set of fixed memory pools, so it is mutex protected for multiple thread usage.
 */
class FixedMemPoolBin
{
  public:
  
    constexpr static size_t MAX_VALUE_SIZE = 8192;
    constexpr static size_t POOL_NUMBER = constLog2(MAX_VALUE_SIZE)-2;

    static FixedMemPoolBin& instance();

    void* allocate(size_t entry_size) noexcept(false);
    void deallocate(void* p) noexcept(false);

#if defined MEM_POOL_DEBUG
    const MemTracker& getTracker() const { return tracker_; }
    MemTracker& getTracker() { return tracker_; }

	void * allocate(size_t entry_size, const char *file, int line)  noexcept(false)
    {
        void * res = allocate(entry_size);
        if (res) tracker_.track (res, file, line);
        return res;
    }
#endif

    ~FixedMemPoolBin();

    void deallocate(size_t bin, void* p) noexcept(false);
    void* allocate(size_t bin, size_t entry_size) noexcept(false);

  private:
#if defined MEM_POOL_DEBUG
    MemTracker  tracker_;
#endif

    FixedMemPool* pools_ [POOL_NUMBER] {nullptr};
    std::mutex                         pools_mutex_;
};

/**
 * \brief Fixed Pool Allocator for std containers such as std::list, std::map and 
 * std::unordered_map. Most std containers are homogeneous containers, i.e. have
 * a fixed element type, but internally allocated memory are not in the same size
 * due to auxiliary structures allocated for connecting elements. Therefore,
 * a set of fixed pools is required to accomplish the allocation.
 */
template <class T>
class StdFixedPoolAllocator
{
  public:

    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_type = T;

    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;

    template <class U> struct rebind { typedef StdFixedPoolAllocator<U> other; };

    StdFixedPoolAllocator(): fixed_mem_pools_(FixedMemPoolBin::instance()) {}

    template <class U>
    StdFixedPoolAllocator(const StdFixedPoolAllocator<U> &other)
        : fixed_mem_pools_ (FixedMemPoolBin::instance())
    {
        //std::cout<< "New rebindAllocator Tsz=" << sizeof(T) << " Usz=" << sizeof(U)
        //<< " T=" << typeid(T).name()
        //<< " U=" << typeid(U).name()
        //<< std::endl;
    }

    pointer allocate(size_type n, const void *hint = 0)
    {
        // if (n != 1 || hint) throw std::bad_alloc();
        // Allocate n number of T instances
        return reinterpret_cast<pointer>(fixed_mem_pools_.allocate(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n)
    {
        fixed_mem_pools_.deallocate(p);
    }

    void construct(pointer p, const_reference val)
    {
        new (p) T(val);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

    pointer address(reference x) const {return &x;}
    const_pointer address(const_reference x) const {return &x;}
    size_type max_size() const throw() {return size_t(-1) / sizeof(value_type);}

  private:
    FixedMemPoolBin&     fixed_mem_pools_;
};

/**
 * \class PooledAllocator
 * \brief Allocator using FixedMemPool for (heterogeneous) non standard containers
 * such as alt::LinkedList, alt::CoQueue and alt::TreeNode.
 */
class PooledAllocator
{
  public:

    static PooledAllocator& instance();

    PooledAllocator() : fixed_mem_pools_(FixedMemPoolBin::instance()) {};
    PooledAllocator(FixedMemPoolBin& pools) : fixed_mem_pools_(pools) {};
    
    template <typename T, typename... Args>
    T* acq(Args&&... args) noexcept(false)
    {
        //constexpr size_t sz_aligned = constAlign(sizeof(T),8);
        static_assert (sizeof(T) <= FixedMemPoolBin::MAX_VALUE_SIZE);
        //constexpr uint32_t pool_index = sz_aligned <= 8 ? 0 : constLog2(sz_aligned-1) - 2;
        void *p = fixed_mem_pools_.allocate(sizeof(T));
        return p ? new(p)T(std::forward<Args>(args)...) : (T*)nullptr;
    }

    template <typename T>
    void del(T* p) noexcept(false)
    {
        if (p)
        {
            //constexpr size_t sz_aligned = constAlign(sizeof(T),8);
            //constexpr uint32_t pool_index = sz_aligned <= 8 ? 0 : constLog2(sz_aligned-1) - 2;
            p->~T();
            fixed_mem_pools_.deallocate((void*)p);
        }
    }

    void* allocate(size_t sz) noexcept(false) { return fixed_mem_pools_.allocate(sz); }
    void deallocate(void* p) noexcept(false) { return fixed_mem_pools_.deallocate(p); }

#if defined MEM_POOL_DEBUG
    template <typename T, typename... Args>
    T* acq(const char *file, int line, Args&&... args) noexcept(false)
    {
        T* res = acq<T>(std::forward<Args>(args)...);
        if (res) fixed_mem_pools_.getTracker().track (res, file, line);
        return res;
    }

	void * allocate(size_t entry_size, const char *file, int line)  noexcept(false)
    {
        return fixed_mem_pools_.allocate(entry_size, file, line);
    }

    const MemTracker& getTracker() const { return fixed_mem_pools_.getTracker(); }
#endif

  private:

    FixedMemPoolBin&     fixed_mem_pools_;
};


/**
 * \class Allocator
 * \brief Allocator using malloc and free non standard containers
 * such as alt::LinkedList, alt::CoQueue and alt::TreeNode.
 */
class Allocator
{
  public:

    static Allocator& instance();

    static void* allocate(size_t entry_size)
    {
        return ::malloc(entry_size);
    }

    static void deallocate(void* p)
    {
        ::free(p);
#if defined MEM_POOL_DEBUG
        tracker_.untrack(p);
#endif
    }
    
    template <typename T, typename... Args>
    static T* acq(Args... args) noexcept(false)
    {
        void *p = allocate(sizeof(T));
        return p ? new(p)T(args...) : (T*)nullptr;
    }

    template <typename T>
    static void del(T* p) noexcept(false)
    {
        if (p) { p->~T(); deallocate((void*)p); }
    }

#if defined MEM_POOL_DEBUG
	static void * allocate(size_t entry_size, const char *file, int line)  noexcept(false)
    {
        void * res = allocate(entry_size);
        if (res) tracker_.track (res, file, line);
        return res;
    }

    template <typename T, typename... Args>
    static T* acq(const char *file, int line, Args... args) noexcept(false)
    {
        T* res = acq<T>(args...);
        if (res) tracker_.track (res, file, line);
        return res;
    }

    static const MemTracker& getTracker() { return tracker_; }
#endif

  private:

#if defined MEM_POOL_DEBUG
    static MemTracker  tracker_;
#endif
};


/**
 * \class PAllocator
 * \brief Allocator using PooledAllocator::instance()
 */
class PAllocator
{
  public:

    static void* allocate(size_t entry_size)
    {
        return PooledAllocator::instance().allocate(entry_size);
    }

    static void deallocate(void* p)
    {
        PooledAllocator::instance().deallocate(p);
    }
    
    template <typename T, typename... Args>
    static T* acq(Args... args) noexcept(false)
    {
        void *p = allocate(sizeof(T));
        return p ? new(p)T(args...) : (T*)nullptr;
    }

    template <typename T>
    static void del(T* p) noexcept(false)
    {
        if (p) { p->~T(); deallocate((void*)p); }
    }

#if defined MEM_POOL_DEBUG
	static void * allocate(size_t entry_size, const char *file, int line)  noexcept(false)
    {
        return PooledAllocator::instance().allocate(entry_size, file, line);
    }

    template <typename T, typename... Args>
    static T* acq(const char *file, int line, Args... args) noexcept(false)
    {
        return PooledAllocator::instance().acq(file, line, args...);
    }

    static const MemTracker& getTracker() { return PooledAllocator::instance().getTracker(); }
#endif
};

} // name space alt

namespace {

#if defined MEM_POOL_DEBUG
#define alt_malloc(len) alt::PooledAllocator::instance().allocate(len,__FILE__,__LINE__)
#define alt_new(T,...) alt::PooledAllocator::instance().template acq<T>(__FILE__,__LINE__,__VA_ARGS__)
#define alt_palloc(pool,len) pool.allocate(len,__FILE__,__LINE__)
#define alt_pnew(pool, T,...) pool.template acq<T>(__FILE__,__LINE__,__VA_ARGS__)
#else
#define alt_malloc(len) alt::PooledAllocator::instance().allocate(len,)
#define alt_new(T,...) alt::PooledAllocator::instance().template acq<T>(__VA_ARGS__)
#define alt_palloc(pool,len) pool.allocate(len)
#define alt_pnew(pool,T,...) pool.template acq<T>(__VA_ARGS__)
#endif
#define alt_free(p) alt::PooledAllocator::instance().deallocate(p)
#define alt_del(T, p) alt::PooledAllocator::instance().template del<T>(p)
#define alt_pfree(pool,p) pool.deallocate(p)
#define alt_pdel(pool,T, p) pool.template del<T>(p)
}

 
