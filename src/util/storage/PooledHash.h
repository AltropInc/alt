#pragma once

#include "LinkedList.h"
#include "FixedMemPool.h"
#include <util/sysinfo/Platform.h>
#include <util/ipc/Mutex.h>
#include <vector>
#include <atomic>
#include <functional>

namespace alt {

// ValueType must provide:
// using KeyType=MyKeyType;
// size_t hashKey();
// const KeyType& key() const;
// static size_t hash(const KeyType& key);
template <typename ValueType, size_t BucketSize=1024>
class PooledHash
{
  public:
  
    /**
     * \struct EntryType
     * \brief Base type for all element types of SharedHash entry.
     */
    struct  EntryType
    {
        template <typename... Args>
        EntryType(Args&&... args) : value_(std::forward<Args>(args)...) {}

        EntryType* next_ {nullptr};
        ValueType  value_;
    };

    using PoolType     = FixedPool<EntryType, BucketSize>;
    using LinkListType = LinkedList<PoolType>;

    template <typename PtrT>
    class IteratorT
    {
        PtrT *       ptr_;
        PooledHash & table_;
        IteratorT(PooledHash & table, PtrT * ptr) : ptr_(ptr), table_(table) {}
        friend class PooledHash;
      public:
        bool operator!=(const IteratorT& itr) const { return ptr_ != itr.ptr_; } 
        ValueType* operator*() const { return &((*ptr_)->value_); }
        void reset (PtrT ptr_t) { *ptr_ = ptr_t; }
        EntryType* getEntry() const { return *ptr_; }
        IteratorT operator++()
        {
            if (!ptr_ || !*ptr_) return IteratorT(ptr_);
            if ((*ptr_)->next_) return &(*ptr_)->next_;
            ptr_ = table_->nextPos(*ptr_);
            return IteratorT(table_, ptr_);
        }
        IteratorT operator++(int)
        {
            auto temp = ptr_;
            if (ptr_ && *ptr_)
            {
                ptr_ = ((*ptr_)->next_) ? &(*ptr_)->next_ : table_->nextPos(*ptr_);
            }
            return IteratorT(table_, temp);
        }
    };
 
    using iterator = IteratorT<EntryType*>;
    using const_iterator = IteratorT<EntryType const*>;

    iterator begin() { return iterator(*this, firstPos()); };
    iterator end() { return iterator(nullptr); };
    const_iterator begin() const { return const_iterator(*this, firstPos()); };
    const_iterator end() const { return const_iterator(nullptr); };

    EntryType const** firstPos() const
    {
        for (size_t ix=0; ix < bucket_number_; ++ix)
        {
            if (buckets_[ix]) return &buckets_[ix];
        }
        return firstPos();
    }

    EntryType** firstPos()
    {
        EntryType const** p = firstPos();
        return const_cast<EntryType**>(p);
    }

    EntryType const** nextPos(EntryType* entry) const
    {
        for (size_t ix=(entry->value_.hashKey() & bucket_ix_mask_)+1; ix < bucket_number_; ++ix)
        {
            if (buckets_[ix]) return &buckets_[ix];
        }
        return nullptr;
    }

    EntryType** nextPos(EntryType* entry)
    {
        EntryType const** p = nextPos();
        return const_cast<EntryType**>(p);
    }

    PooledHash (PoolType& pool, size_t bucket_number=1024) : pool_(pool)
    {
        bucket_number_ = power2Next<size_t>(bucket_number);
        bucket_ix_mask_ = bucket_number_-1;
        buckets_.resize(bucket_number);
    }

    ValueType* insert(EntryType* entry)
    {
        auto& header = buckets_[entry->value_.hashKey() & bucket_ix_mask_];
        entry->next_ = header;
        header = entry;
        return &entry->value_;
    }

    template <typename... Args>
    ValueType* emplace(Args&&... args)
    {
        EntryType* entry = pool_.acq(std::forward<Args>(args)...);
        return insert(entry);
    }

    bool erase(typename ValueType::KeyType const&& key)
    {
        auto& header = buckets_[ValueType::hash(key) & bucket_ix_mask_];
        EntryType* n = reinterpret_cast<EntryType*>(header);
        EntryType* prev_n {nullptr};
        for (; n; prev_n = n, n = n->next_)
        {
            if (n->value_.key()==key)
            {
                extract(n, prev_n, header);
                pool_.del(n);
                return true;
            }
        }
        return false;
    }

    bool erase(EntryType* entry)
    {
        auto& header = buckets_[entry->value_.hashKey() & bucket_ix_mask_];
        EntryType* n = header;
        EntryType* prev_n {nullptr};
        for (; n; prev_n = n, n = n->next_)
        {
            if (n==entry)
            {
                extract(n, prev_n, header);
                pool_.del(entry);
                return true;
            }
        }
        return false;
    }

    ValueType* findValue(typename ValueType::KeyType const& key)
    {
        EntryType* n = reinterpret_cast<EntryType*>(buckets_[ValueType::hash(key) & bucket_ix_mask_]);
        while (n)
        {
            if (key==n->value_.key()) 
            {
                return &n->value_;
            }
            n = reinterpret_cast<EntryType*>(n->next_);
        }
        return nullptr;
    }

    ValueType* replaceKey(typename ValueType::KeyType const& key,
                  typename ValueType::KeyType const& new_key)
    {
        auto& header = buckets_[ValueType::hash(key) & bucket_ix_mask_];
        EntryType* n = reinterpret_cast<EntryType*>(header);
        EntryType* prev_n {nullptr};
        for (; n; prev_n = n, n = reinterpret_cast<EntryType*>(n->next_))
        {
            if (key==n->value_.key())
            {
                extract(n, prev_n, header);
                n->value_.resetKey(new_key);
                n->next_ = nullptr;
                return insert(n);
            }
        }
        return nullptr;
    }

    iterator find(typename ValueType::KeyType const& key)
    {
        auto& header = buckets_[ValueType::hash(key) & bucket_ix_mask_];
        EntryType* n = reinterpret_cast<EntryType*>(header);
        EntryType* prev_n {nullptr};
        for (; n; prev_n = n, n = reinterpret_cast<EntryType*>(n->next_))
        {
            if (key==n->value_.key())
            {
                return iterator(*this, 
                       prev_n ? reinterpret_cast<EntryType**>(&prev_n->next_)
                              : reinterpret_cast<EntryType**>(&header));
            }
            n = reinterpret_cast<EntryType*>(n->next_);
        }
        return iterator(*this, nullptr);
    }

    void erase(iterator iter)
    {
        EntryType* entry = iter.getEntry();
        iter.reset(reinterpret_cast<EntryType*>(entry->next_));
        pool_.del(entry);
    }

  private:

    inline static void extract (
        EntryType* n,
        EntryType* prev_n,
        EntryType*& header)
    {
        if (prev_n)
        {
            prev_n->next_ = n->next_;
        }
        else
        {
            header = n->next_;
        }
    }

    std::vector<EntryType*>             buckets_;
    PoolType &                          pool_;
    size_t                              bucket_number_;
    size_t                              bucket_ix_mask_;
};

}