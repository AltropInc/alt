#include "Allocator.h"
#include <util/string/StrBuffer.h>
#include <util/string/StrPrint.h>

#define ALLOCATE_DEBUG    0

#if defined MEM_POOL_DEBUG
#include <iostream>
#include <unordered_map>
#endif

#if defined MEM_POOL_DEBUG
namespace alt
{
    struct FileLinePair
    {
        StrFixed<32> file_;
        int          line_ { 0 };
        static const char* getFileName(const char* path)
        {
            const char * cp = std::strrchr(path, '/');
            if (!cp) cp = std::strrchr(path, '\\');
            return cp ? ++cp : path;
        }
        
        bool operator==(const FileLinePair &other) const
        { 
            return file_ == other.file_ && line_==other.line_;
        }

        FileLinePair() {}
        FileLinePair(const char* file, int line): file_(getFileName(file)), line_(line)
        {}
        FileLinePair(const FileLinePair& oth): file_(oth.file_), line_(oth.line_)
        {}
        FileLinePair(const FileLinePair&& oth): file_(oth.file_), line_(oth.line_)
        {}

        size_t hash() const
        {
            size_t seed = 0;
            {
                seed = size_t(line_);
                seed ^= file_.hash() + 0x9e3779b9 + (seed<<6) + (seed>>2);
            }
            return seed;
        }
    };
}

namespace std {
template <>
struct hash<alt::FileLinePair>
{
    size_t operator()(const alt::FileLinePair& key) const noexcept
    {
        return key.hash();
    }
};
} // namespace std

#endif
namespace alt
{
#if defined MEM_POOL_DEBUG
//-----------------------------------------------------------------------------
// Mem Tracker
//-----------------------------------------------------------------------------
struct MemTracker::MemTrackerImpl
{
    struct TrackRecord
    {
        unsigned     count_         {0};
        const char*  file_          {nullptr};
        int          line_          {0};
        constexpr TrackRecord() {};
    };
    std::unordered_map<void*, TrackRecord*> mem_map_;
    std::unordered_map<FileLinePair, TrackRecord> alloc_map_;
    TrackRecord*       most_used_{nullptr};
    uint64_t           total_count_ { 0L };
};

MemTracker::MemTracker(): impl_ (new MemTrackerImpl())
{ }

MemTracker::~MemTracker() { delete impl_; }

uint64_t MemTracker::getTotalCount() const { return impl_->total_count_; }

// This is use to track memory leak
size_t MemTracker::reportMostUsed(char* buffer, size_t buffer_sz) const
{
    StrBuf str_buf(buffer, buffer_sz);
    StrPrint<StrBuf> spr(str_buf);
    spr << "[PooledAllocator Usage] Total cnt=" << impl_->total_count_;
    if (impl_->most_used_)
    {
        spr << ", most used by " << impl_->most_used_->file_ << ':'
            << impl_->most_used_->line_ << " cnt="
            << impl_->most_used_->count_;
    }
    spr << '\0';
    return spr.length();
}

void MemTracker::reportMostUsed() const
{
    char info[128];
    reportMostUsed(info,sizeof(info));
    std::cout << info << std::endl;
}

void MemTracker::untrack(void * p)
{
    auto iter = impl_->mem_map_.find(p);
    if (iter!=impl_->mem_map_.end())
    {
        iter->second->count_--;
        --impl_->total_count_;
        if (iter->second->count_<=0)
        {
            if (impl_->most_used_ && iter->second->file_==impl_->most_used_->file_ &&
                iter->second->line_==impl_->most_used_->line_)
            {
                impl_->most_used_=nullptr;
            }
            impl_->alloc_map_.erase(FileLinePair(iter->second->file_, iter->second->line_));
        }
        impl_->mem_map_.erase(iter);
    }
}

void MemTracker::track(void * p, const char *file, int line)
{
    std::unordered_map<FileLinePair, MemTrackerImpl::TrackRecord>::iterator iter;
    bool inserted {false};
    std::tie(iter, inserted) = impl_->alloc_map_.emplace
        (FileLinePair(file, line),MemTrackerImpl::TrackRecord());
    if (inserted)
    {
        iter->second.file_ = iter->first.file_.c_str();
        iter->second.line_ = iter->first.line_;
    }
    ++(iter->second.count_);
    ++impl_->total_count_;
    impl_->mem_map_.emplace(p, &(iter->second));
    if (!impl_->most_used_ || iter->second.count_ > impl_->most_used_->count_)
    {
        impl_->most_used_ = &(iter->second);
        // std::cout << "set most used tp " << iter->second.file_ << std::endl;
    }
}

MemTracker Allocator::tracker_;
#endif

FixedMemPoolBin& FixedMemPoolBin::instance()
{
    static FixedMemPoolBin s_fixed_mem_pool_bin;
    return s_fixed_mem_pool_bin;
}

void FixedMemPoolBin::deallocate(size_t bin, void* p)
{
#if ALLOCATE_DEBUG
    std::cout << "FixedMemPoolBin::deallocate bin=" << bin << " p=" << p << std::endl;
    if (bin < POOL_NUMBER)
        std::cout << "FixedMemPoolBin::deallocate pools_[bin]=" << pools_[bin] << std::endl;
#endif
    if (bin >= POOL_NUMBER || nullptr == pools_[bin])
    {
        throw std::runtime_error(std::string("PooledAllocator::deallocate: currupted memory"));
    }
#if defined MEM_POOL_DEBUG
    tracker_.untrack(p);
#endif
    reinterpret_cast<FixedMemPool*>(pools_[bin])->coDeallocate(p);
}

void* FixedMemPoolBin::allocate(size_t bin, size_t entry_size)
{
#if ALLOCATE_DEBUG
    std::cout << "PooledAllocator::allocate: entry_size="
    << entry_size << " in bin " << bin
    << " pools_[bin]= " << pools_[bin] << std::endl;
#endif
    if (nullptr == pools_[bin])
    {
#if defined IN_UNIT_TEST
        // Use small number to test new bucket allocation in unit test
        size_t entry_num_per_bucket = 4;
#else
        size_t entry_num_per_bucket = (POOL_NUMBER-bin)*100;
#endif
        std::scoped_lock<std::mutex> lock(pools_mutex_);
        if (nullptr == pools_[bin])
        {
            pools_[bin] = new FixedMemPool(1<<(bin+3), entry_num_per_bucket);
        }
    }

#if ALLOCATE_DEBUG
    void* p= pools_[bin]->allocate(bin);
    std::cout << "allocated " << p << std::endl;
    return p;
#else
    return pools_[bin]->coAllocate(bin);
#endif
}

void* FixedMemPoolBin::allocate(size_t size)
{
    int bin = size <= 8 ? 0 : log2Floor(size-1) - 2;
#if ALLOCATE_DEBUG
    std::cout << "Allocated bin=" << bin
              << " at size " << size
              << " log2Floor(size) " << log2Floor(size)
              << std::endl;
#endif
    if (bin >= POOL_NUMBER)
    {
        return FixedMemPool::allocateBigSize(size, POOL_NUMBER);
    }

    return allocate(bin, size);
}

void* FixedMemPoolBin::reallocate(void* p, size_t new_size)
{
    if (!p)
    {
        return allocate(new_size);
    }
    uint16_t old_bin = FixedMemPool::getAlloactedBin(p);
    int new_bin = new_size <= 8 ? 0 : log2Floor(new_size-1) - 2;
    if (old_bin > POOL_NUMBER && new_bin > POOL_NUMBER)
    {
        return FixedMemPool::reallocateBigSize(p, new_size, new_bin);
    }
    if ((old_bin==new_bin || new_bin == old_bin-1) && old_bin < POOL_NUMBER)
    {
        // no need to allocate if both are allocate in bins and the new bin
        // is not bigger nor smaller by more than one.
        return p;
    }

    // for all other case, we nee reallocate and copy the memory contents
    void* new_buffer =  (new_bin >= POOL_NUMBER)
        ? FixedMemPool::allocateBigSize(new_size, POOL_NUMBER)
        : allocate(new_bin, new_size);

    size_t sz = old_bin > POOL_NUMBER ? new_size : std::min(new_size, pools_[old_bin]->slotSize());
    memcpy(new_buffer, p, sz);
    return new_buffer;
}

void FixedMemPoolBin::deallocate(void* p)
{
#if ALLOCATE_DEBUG
    std::cout << "FixedMemPoolBin::deallocate p=" << p << std::endl;
#endif
    uint16_t bin = FixedMemPool::getAlloactedBin(p);
    if (bin >= POOL_NUMBER)
    {
        FixedMemPool::deallocateBigSize(p);
    }
    else
    {
        deallocate(bin, p);
    }
}

FixedMemPoolBin::~FixedMemPoolBin( )
{
    for (size_t i=0; i<POOL_NUMBER; ++i)
    {
        delete pools_[i];
        pools_[i] = nullptr;
    }
}

PooledAllocator& PooledAllocator::instance()
{
    static PooledAllocator s_pooled_allocator;
    return s_pooled_allocator;
}

Allocator& Allocator::instance()
{
    static Allocator s_allocator;
    return s_allocator;
}

} // name space alt

 
