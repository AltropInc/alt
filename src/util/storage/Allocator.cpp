#include "Allocator.h"
#include <util/string/StrBuffer.h>
#include <util/string/StrPrint.h>

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

void FixedMemPoolBin::free(size_t bin, void* p)
{
    if (bin >= POOL_NUMBER || nullptr == pools_[bin])
    {
        throw std::runtime_error(std::string("PooledAllocator::fedd: currupted memory"));
    }
#if defined MEM_POOL_DEBUG
    tracker_.untrack(p);
#endif
    reinterpret_cast<FixedMemPool*>(pools_[bin])->free(p);
}

void* FixedMemPoolBin::alloc(size_t bin, size_t entry_size)
{
    // std::cout << "PooledAllocator::alloc: entry_size=" << entry_size << " in bin " << bin << std::endl;
    if (nullptr == pools_[bin])
    {
#if defined IN_UNIT_TEST
        // Use small number to test new bucket allocation in unit test
        size_t entry_num_per_bucket = 4;
#else
        size_t entry_num_per_bucket = (POOL_NUMBER-bin)*100;
#endif
        pools_[bin] = new FixedMemPool(entry_size, entry_num_per_bucket);
    }
    return pools_[bin]->alloc(bin);
}

void* FixedMemPoolBin::alloc(size_t size)
{
    int bin = size <= 8 ? 0 : log2Floor(size) - 2;
    // std::cout << "Allocated bin=" << bin << " at size " << size << std::endl;
    if (bin >= POOL_NUMBER)
    {
        throw std::runtime_error(std::string("FixedMemPoolBin::alloc: size too big"));;
    }

    return alloc(bin, size);
}

void FixedMemPoolBin::free(void* p)
{
    free(FixedMemPool::getAlloactedBin(p), p);
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

 
