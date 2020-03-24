#include "FixedMemPool.h"
#include <util/string/StrBuffer.h>
#include <util/string/StrPrint.h>

#include <cstdlib>        // for malloc and free
#include <stdexcept>      // for runtime_error
#include <string>         // for string used by runtime_error
#include <iostream>

#if defined MEM_POOL_DEBUG
#define MEM_POOL_DBG(x)      x
#else
#define MEM_POOL_DBG(x)
#endif

namespace alt
{
//-----------------------------------------------------------------------------
// class FixedMemPool
//-----------------------------------------------------------------------------
struct FixedMemPool::EntryHeader
{
    union
    {
        EntryHeader*       next_free_entry_;
        uint64_t           uint64_header_;
        struct
        {
            uint64_t        magic_word_   : 16;  // 0XA3C5
            uint64_t        bin_          : 16;
            uint64_t        slab_ix_      : 32;
        }
        header_struct_;
    };

    static constexpr uint16_t MAGIC_WORD = 0XA3C5;
#if (ALT_ENDIAN == ALT_ENDIAN_BIG)
    static constexpr uint64_t INITIAL_HEADER_VALUE = 0xC5A3000000000001ULL;
#else
    static constexpr uint64_t INITIAL_HEADER_VALUE = 0X10000A3C5ULL;
#endif
    void setAllocated(uint16_t bin)
    {
        uint64_header_ = INITIAL_HEADER_VALUE;
        header_struct_.bin_ = bin;
    }
};

FixedMemPool::FixedMemPool(size_t value_size, size_t slot_num_per_slab, bool lazy_alloc)
    : value_size_(value_size)
    , slot_size_ (sizeof(EntryHeader)+constAlign(value_size,8))
    , slot_num_per_slab_(slot_num_per_slab)
{ 
    MEM_POOL_DBG((std::cout << "FixedMemPool(" << slot_size_ << ") Constructor lazy_alloc=" << lazy_alloc << std::endl));
    if (!lazy_alloc)
    {  
        head_ = newSlab();
        MEM_POOL_DBG((std::cout << "FixedMemPool(" << slot_size_ << ") newSlab head_" << head_<<  std::endl));
    } 
}

FixedMemPool::~FixedMemPool()
{
    MEM_POOL_DBG((std::cout << "~FixedMemPool(" << slot_size_ << ") called" <<  std::endl));
    for (auto slab: slab_list_)
    {
        ::free(slab);
    }
}

void* FixedMemPool::alloc(uint16_t bin)
{
    if (!head_)
    {
        head_ = newSlab();
        MEM_POOL_DBG((std::cout << "FixedMemPool(" << slot_size_ << ") newSlab in alloc head_ " << head_<<  std::endl));
    }

    EntryHeader* cur_head = head_;
    head_ = cur_head->next_free_entry_;
    MEM_POOL_DBG((std::cout << "FixedMemPool(" << slot_size_ << ") new head in alloc head_ " << head_
                    << " allocated=" << cur_head
                    << " next of head=" << head_->next_free_entry_
                    << std::endl));
    cur_head->setAllocated(bin);
    return cur_head+1;
}

uint16_t FixedMemPool::getAlloactedBin(void* p)
{
    EntryHeader* cur_head =
        reinterpret_cast<EntryHeader*>(p) - 1;
    return cur_head->header_struct_.bin_;
}

void FixedMemPool::free(void* p)
{
    EntryHeader* cur_head = reinterpret_cast<EntryHeader*>(p) - 1;

    if (EntryHeader::MAGIC_WORD != cur_head->header_struct_.magic_word_)
    {
        MEM_POOL_DBG((std::cout << "FixedMemPool(" << slot_size_ << ") ::free: memory currupted bin="
            << cur_head->header_struct_.bin_ << std::endl));
        throw std::runtime_error(std::string("FixedMemPool::free: memory currupted"));
    }
    cur_head->next_free_entry_ = head_;
    head_ = cur_head;
    MEM_POOL_DBG((std::cout << "FixedMemPool(" << slot_size_ << ") head after free head_=" << head_
        << " prev head= " << cur_head->next_free_entry_
        << std::endl));
}

FixedMemPool::EntryHeader* FixedMemPool::newSlab()
{
    MEM_POOL_DBG((std::cout << "FixedMemPool::newSlab: malloc size="
              << slot_size_*slot_num_per_slab_
              << " slot_size="  << slot_size_
              << std::endl));
    void* slab = ::malloc(slot_size_*slot_num_per_slab_);
    if (!slab)
    {
        throw std::runtime_error(std::string("FixedMemPool::newSlab: memory full"));
    }
    uint8_t* p = reinterpret_cast<uint8_t*>(slab);
    EntryHeader* entry=reinterpret_cast<EntryHeader*>(p);
    for ( int i=0; i<slot_num_per_slab_-1; ++i )
    {
        p+=slot_size_;
        entry->next_free_entry_ = reinterpret_cast<EntryHeader*>(p);
        MEM_POOL_DBG((if (i < 10)
           std::cout << "FixedMemPool(" << slot_size_ << ")::newSlab next_free_entry_"
           << entry->next_free_entry_  << std::endl));
        entry=reinterpret_cast<EntryHeader*>(p);
    }
    entry->next_free_entry_ = nullptr;
    slab_list_.push_back(slab);
    return reinterpret_cast<EntryHeader*>(slab);
}


FixedMemPoolPrealloc::PoolHeader::PoolHeader(char* addr, size_t slot_num, size_t value_size)
    : owns_buffer_(false)
    , value_size_(value_size)
    , slot_size_ (constAlign(value_size,8))
    , slot_num_(slot_num)
    , addr_ (addr)
{}

FixedMemPoolPrealloc::PoolHeader::PoolHeader(size_t slot_num, size_t value_size)
    : owns_buffer_(true)
    , value_size_(value_size)
    , slot_size_ (constAlign(value_size,8))
    , slot_num_(slot_num)
    , addr_ (reinterpret_cast<char*>(::malloc(slot_size_*slot_num_)))
{}

struct FixedMemPoolPrealloc::EntryHeader
{
    EntryHeader*  next_free_entry_;
};

void FixedMemPoolPrealloc::PoolHeader::initialize()
{
    uint8_t* p = reinterpret_cast<uint8_t*>(addr_);
    EntryHeader* entry=reinterpret_cast<EntryHeader*>(p);
    for ( int i=0; i<slot_num_-1; ++i )
    {
        p+=slot_size_;
        entry->next_free_entry_ = reinterpret_cast<EntryHeader*>(p);
        entry=reinterpret_cast<EntryHeader*>(p);
    }
    entry->next_free_entry_ = nullptr;
}

FixedMemPoolPrealloc::FixedMemPoolPrealloc(char* addr, size_t slot_num, size_t value_size)
    : header_(addr, slot_num, value_size)
{}

FixedMemPoolPrealloc::FixedMemPoolPrealloc(size_t slot_num, size_t value_size)
    : header_(slot_num, value_size)
{}

void FixedMemPoolPrealloc::setAddr(char* addr, size_t value_size, size_t slot_num)
{
    header_.addr_ = addr;
    header_.value_size_ = value_size;
    header_.slot_size_ = constAlign(value_size,8);
    header_.slot_num_ = slot_num;
    header_.initialize();
}

FixedMemPoolPrealloc::~FixedMemPoolPrealloc()
{
    if (header_.owns_buffer_)
    {
        ::free(header_.addr_);
    }
}

void* FixedMemPoolPrealloc::alloc()
{
    EntryHeader* expected_free_entry_head = header_.head_.load(std::memory_order_relaxed);
    EntryHeader* next_free_entry;
    do
    {
        if (!header_.head_)
        {
            return nullptr;
        }

        next_free_entry = expected_free_entry_head->next_free_entry_;
    }
    while (!header_.head_.compare_exchange_weak(expected_free_entry_head, next_free_entry,
                std::memory_order_release, std::memory_order_acquire));

    return expected_free_entry_head;
}

void FixedMemPoolPrealloc::free(void* p)
{
    EntryHeader* new_head = reinterpret_cast<EntryHeader*>(p);
    EntryHeader* cur_head = header_.head_;
    do
    {
        new_head->next_free_entry_ = cur_head;
    }
    while (!header_.head_.compare_exchange_weak(cur_head, new_head,
            std::memory_order_release, std::memory_order_acquire));
}

} // name space alt

 
