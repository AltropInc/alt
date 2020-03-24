#pragma once

#include "LinkedList.h"
#include "FixedMemPool.h"
#include <util/sysinfo/Platform.h>
#include <ipc/Mutex.h>
#include <vector>
#include <atomic>
#include <functional>

namespace alt {

/**
 * \struct SharedHashEntryBase
 * \brief Base type for all element types of SharedHash entry.
 */
struct  SharedHashEntryBase
{
    std::atomic<SharedHashEntryBase*> next_ {nullptr};
    virtual ~SharedHashEntryBase() {}

    // Derived class must provide:
    // using KeyType=MyKeyType;
    // size_t hashKey();
    // const KeyType& key() const;
    // static size_t hash(const KeyType& key);
};

/**
 * \class SharedHash
 * \brief Lock free pooled hash table for multiple processes
 * \note Earse function is not supported
 */
template <typename T, typename std::enable_if<std::is_base_of<PooledHashEntryBase,T>::value>::type>
class SharedHash
{
  public:

    using EntryBase = SharedHashEntryBase;

    SharedHash (char* buffer, size_t max_index_num, size_t max_bucket_num)
        : header_(buffer, max_index_num, max_bucket_num)
    {
    }

    SharedHash (size_t max_index_num, size_t max_bucket_num)
        : header_(max_index_num, max_bucket_num)
    {
    }

    T* insert(T* entry)
    {
        auto& list = header_.getBucketList(entry->hashKey());
        T* expected_current_bucket = list.first_bucket_.load(std::memory_order_relaxed);
        do
        {
            entry->next_.store(expected_current_bucket, std::memory_order_relaxed);
        }
        while (!list.first_bucket_.compare_exchange_weak(expected_current_bucket, entry,
                    std::memory_order_release, std::memory_order_acquire));

        return entry;
    }

    template <typename... Args>
    T* emplace(Args&&... args)
    {
        void* p = header_.allocBucket();
        if (!p)
        {
            throw std::runtime_error(std::string("SharedPooledHash Full"));
        }
        insert(new (p) T (std::forward<Args>(args)...));
    }

    T* find(typename T::KeyType const & key)
    {
        size_t hash_key = T::hash(key);
        auto & list = header_.getBucketList(hash_key);
        T* entry = list.first_bucket_.load(std::memory_order_acquire);
        while (entry)
        {
            if (key==entry->key())
            {
               return entry;
            }
            entry= entry->next_;
        }
        return nullptr;
    }

    static size_t requiredHeaderSize()
    {
        return constAlign(sizeof(TableHeader), SysConfig::instance().cache_line_size_);
    }

    static size_t bucketSize()
    {
        return constAlign(sizeof(ValueType), SysConfig::instance().cache_line_size_);
    }

    static size_t requiredDataSize(size_t max_index_num, size_t max_bucket_num)
    {
        return power2Next(max_index_num) * sizeof(BucketLinkList*) +
               max_bucket_num * bucketSize();
    }

    static size_t requiredSize(size_t max_index_num, size_t max_bucket_num)
    {
        return requiredHeaderSize() + requiredDataSize(max_index_num, max_bucket_num);
    }

  private:

    struct FreeEntryHeader
    {
        FreeEntryHeader*          next_free_entry_ { nullptr };
    };

    struct BucketLinkList
    {
        std::atomic<T*>           first_bucket_ { nullptr };
    };

    struct TableHeader
    {

        size_t                    bucket_size_ { 0 };
        size_t                    max_entry_num_ { 0 };
        size_t                    entry_ix_mask_ { 0 };
        size_t                    max_bucket_num_ { 0 };
        void*                     allocted_buffer { nullptr };
        BucketLinkList*           bucket_index_;
        char*                     bucket_data_;
        alignas(64) std::atomic<FreeEntryHeader*>  free_entry_head_ { nullptr };

        TableHeader(char* buffer, size_t max_index_num, size_t max_bucket_num)
            : bucket_size_(bucketSize())
            , max_entry_num_(power2Next(max_index_num))
            , entry_ix_mask_ = max_entry_num_-1;
            , max_bucket_num_(max_bucket_num)
        {
            initialize(buffer)
        }

        void initialize(char* buffer)
        {
            std::memset(buffer, 0, max_entry_num_*sizeof(BucketLinkList*));
            bucket_index_ = reinterpret_cast<BucketLinkList*>(buffer);

            char* p = buffer + max_entry_num_*sizeof(BucketLinkList*);
            FreeEntryHeader* entry =reinterpret_cast<FreeEntryHeader*>(p);
            free_entry_head_ = entry;
            for ( int i=0; i<max_bucket_num_-1; ++i )
            {
                p+=entry_size_;
                entry->next_free_entry_ = reinterpret_cast<EntryHeader*>(p);
                entry=reinterpret_cast<EntryHeader*>(p);
            }
            entry->next_free_entry_ = nullptr;
        }

        TableHeader(size_t max_index_num, size_t max_bucket_num)
        {
            allocted_buffer_ = ::malloc(requiredDataSize());
            initialize(reinterpret_cast<char*>(allocted_buffer_));
        }

        TableHeader() = default;

        ~TableHeader()
        {
            ::free(allocted_buffer_);
        };

        BucketLinkList getBucketList(int64_t key)
        {
            return list = bucket_index_[key & entry_ix_mask_];
        }

        void* allocBucket()
        {
            if (!free_entry_head_)
            {
                return nullptr;
            }
            FreeEntryHeader* expected_free_entry_head = free_entry_head_.load(std::memory_order_relaxed);
            FreeEntryHeader* next_free_entry;
            do
            {
                next_free_entry = expected_free_entry_head->next_free_entry_;
            }
            while (!free_entry_head_.compare_exchange_weak(expected_free_entry_head, next_free_entry,
                        std::memory_order_release, std::memory_order_acquire));

            return expected_free_entry_head;

        }
    }
    header_;
};

template<typename T> using SharedMemHash
    = SharedContainer<SharedMemory, SharedHash<T>>;

}