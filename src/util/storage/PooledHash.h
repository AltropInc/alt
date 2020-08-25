#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file PooledHash.h
 * @library alt_util
 * @brief Defines  a hush table use fixed memory pool.
 * PooledHash is similar to:
 *    - std::unordered_map<T, alt::StdFixedPoolAllocator<std::pair<const Key, T>>>,
 * but has the following advantages:
 *    - less fragments: the key and the link pointers are packed together with the value
 *    - less cache misses: uses a single fixed pool. std::unordered_map has to rebind
 *      allocator and using multiple fixed pool for different allocate sizes
 *    - choice of pool: you can provide dedicated fixed pool instead of using the singleton
 *      of the memory pool shared with all others
 *    - more efficient
 * Besides, there is one important difference:
 *    - the map may contain mutiple values with the same key if Uniqueness is false
 *      unless you guarantee that all values inserted have unique keys.
 */

#include "LinkedList.h"                 // for LinkedList
#include "FixedMemPool.h"               // for FixedPool
#include <util/numeric/Intrinsics.h>    // for power2Next
#include <util/types/TemplateHelper.h>  // for NONCOPYABLE
#include <type_traits>                  // for is_trivially_destructible

#define MAKE_POOLED_HASH_ENTRY(KT, KF, KFUNC) \
    using KeyType = KT; \
    size_t hashKey() const { return KF; } \
    const KeyType& key() const { return KF; } \
    void resetKey(KeyType new_key) { KF=new_key; } \
    static size_t hash(const KeyType& key) { return KFUNC(key); }

namespace alt {

/**
 * \class PooledHash
 * \ingroup ContainerUtils
 * \brief  Implements a hush table use fixed memory pool.
 * \tparam ValueType the type of the value, which contains the key. The ValueType must
 * provide the following:
 *    - using KeyType=MyKeyType;
 *    - size_t hashKey();
 *    - const KeyType& key() const;
 *    - static size_t hash(const KeyType& key);
 *    - void resetKey(KeyType new_key)
 * In most cases, you can use macro MAKE_POOLED_HASH_ENTRY to provide the above functions.
 * \tparam Uniqueness tells whether the uniqueness of the key should be ensured.
 * If you know for sure all values inserted into the map have unique keys, and can
 * leave Uniqueness false so the map will not check uniqueness for each insert and
 * this will make the map more efficient
 * \tparam BucketSize the size of the bucket of the fixed memory pool.
 */
template <typename ValueType, bool Uniqueness=false, size_t BucketSize=1024>
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
        
        EntryType(const ValueType& value) : value_(value) {}

        EntryType* next_ {nullptr};
        ValueType  value_;
    };

    using value_type     = ValueType;
    using entry_type     = EntryType;
    using allocator_type = FixedPool<EntryType, BucketSize>;

    /**
     * \struct InsertResult
     * \brief The result returned by insert and replaceKkey
     */
    struct InsertResult
    {
        ValueType*   value_;   ///< pointer to the value inserted
        bool         is_new_;  ///< true if the value is newly inseted or the key is replaced
    };

    NONCOPYABLE(PooledHash);
    
    /// \brief constructor
    /// \param pool a fixed pool to hold all elements inserted into the map
    /// if not given, a pool will be created internally. Provide a pool if
    /// you want share a pool among multiple maps tof the same value type.
    /// \param bucket_number the number of the buckets. The larger, the less
    /// chance of collision.
    PooledHash (allocator_type* pool=nullptr, size_t bucket_number=1024) : pool_(pool)
    {
        if (!pool_)
        {
            pool_ = new allocator_type();
            owns_pool_ = true;
        }
        bucket_number_ = power2Next<size_t>(bucket_number);
        bucket_ix_mask_ = bucket_number_-1;
        buckets_.resize(bucket_number);
    }

    /// \brief destructor
    ~PooledHash()
    {
        if (!std::is_trivially_destructible<ValueType>::value || !owns_pool_)
        {
            for (auto entry : *this) pool_->del(entry);
        }
        if (owns_pool_)
        {
            delete pool_;
        }
    }

    /// \brief Forward iterator that can read data from the pointed-to element
    /// and forward to the position of the next element
    template <typename PtrT>
    class IteratorT
    {
        PtrT *       ptr_;
        PooledHash & table_;
        IteratorT(PooledHash & table, PtrT * ptr) : ptr_(ptr), table_(table) {}
        friend class PooledHash;
      public:
        bool operator!=(const IteratorT& itr) const { return ptr_ != itr.ptr_; } 
        //ValueType* operator*() const { return &((*ptr_)->value_); }
        EntryType* operator*() const { return *ptr_; }
        void reset (PtrT ptr_t) { *ptr_ = ptr_t; }
        EntryType* getEntry() const { return *ptr_; }
        const ValueType& getValue() const { return getEntry()->value_; }
        ValueType& getValue() { return getEntry()->value_; }
        IteratorT operator++()
        {
            if (!ptr_ || !*ptr_) return IteratorT(table_, ptr_);
            if ((*ptr_)->next_) return IteratorT(table_, &(*ptr_)->next_);
            ptr_ = table_.nextPos(*ptr_);
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
 
    /// \brief Forward iterator
    using iterator = IteratorT<EntryType*>;

    /// \brief Const forward iterator
    using const_iterator = IteratorT<EntryType const*>;

    /// \brief Returns an iterator to the first element or the iterator equal to
    /// end() if the nap is empty.
    iterator begin() { return iterator(*this, firstPos()); };
    const_iterator begin() const { return const_iterator(*this, firstPos()); };

    /// \brief Returns an iterator represents the position beyond the last
    /// element of the map.
    iterator end() { return iterator(*this, nullptr); };
    const_iterator end() const { return const_iterator(*this, nullptr); };

    /// \brief Inserts an entry constructed outside
    /// \return the InsertResult with a pointer to the memory where the value
    ///  is stored and a boolean value to indicate whether the value is newly
    /// inserted.
    InsertResult insert(EntryType* entry)
    {
        auto& header = buckets_[entry->value_.hashKey() & bucket_ix_mask_];
        if (Uniqueness && header)
        {
            for (auto n = header; n; n= n->next_)
            {
                if (n->value_.key()==entry->value_.key())
                {
                    pool_->del(entry);
                    return {&n->value_, false};
                }
            }
        }
        entry->next_ = header;
        header = entry;
        return {&entry->value_,true};
    }

    /// \brief Inserts an value
    /// \return the InsertResult with a pointer to the memory where the value
    ///  is stored and a boolean value to indicate whether the value is newly
    /// inserted.
    InsertResult insert(const ValueType& value)
    {
        void* p = pool_->FixedMemPool::allocate();
        EntryType* entry = new (p) EntryType(value);
        return insert(entry);
    }

    /// \brief Inserts a new element into the map constructed in-place
    /// with the given args if there is no element with the key in the container.
    /// \return the InsertResult with a pointer to the memory where the value
    ///  is stored and a boolean value to indicate whether the value is newly
    /// inserted.
    template <typename... Args>
    InsertResult emplace(Args&&... args)
    {
        EntryType* entry = pool_->acq(std::forward<Args>(args)...);
        return insert(entry);
    }

    bool erase(typename ValueType::KeyType const& key)
    {
        auto& header = buckets_[ValueType::hash(key) & bucket_ix_mask_];
        EntryType* n = reinterpret_cast<EntryType*>(header);
        EntryType* prev_n {nullptr};
        for (; n; prev_n = n, n = n->next_)
        {
            if (n->value_.key()==key)
            {
                extract(n, prev_n, header);
                pool_->del(n);
                return true;
            }
        }
        return false;
    }


    /// \brief Removes the element in the given entry.
    /// \return true if the value is removed, false if the value is not found
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
                pool_->del(entry);
                return true;
            }
        }
        return false;
    }

    /// \brief Removes the element at pos. References to the erased elements
    /// are invalidated
    /// \return iterator to the next element
    iterator erase(iterator pos)
    {
        EntryType* entry = pos.getEntry();
        pos.reset(reinterpret_cast<EntryType*>(entry->next_));
        pool_->del(entry);
        return pos;
    }

    /// \brief find the first value by its key value
    /// \return the pointer to the memory where the value is stored. If no such
    /// element is found, nullptr is returned.
    /// \note Use this if you just need to find the value. If you want to find
    /// the value position for erase operation, call findEntry to get the entry
    /// node, or for eras and position forwarding operation, call find to get
    /// the iterator
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

    /// \brief find the first entry node by value's key value
    /// \return the pointer to the entry where the value is stored. If no such
    /// element is found, nullptr is returned.
    /// \note If you want to find the iterator position, call find
    EntryType* findEntry(typename ValueType::KeyType const& key)
    {
        EntryType* n = reinterpret_cast<EntryType*>(buckets_[ValueType::hash(key) & bucket_ix_mask_]);
        while (n)
        {
            if (key==n->value_.key()) 
            {
                return n;
            }
            n = reinterpret_cast<EntryType*>(n->next_);
        }
        return nullptr;
    }

    /// \brief find the first element by its key value
    /// \return iterator to the element found.  If no such element is found,
    /// past-the-end (see end()) iterator is returned
    /// \note If values in the map contains the entries of the same leys, call
    /// this function to get the iterator so that you can check if the value at
    /// the next position have the same key
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

    /// \brief replace the element key value
    /// \return the InsertResult having a pointer to the memory where the value's
    ///  key is renewed and a boolean value to indicate whether the key is newly
    /// replaced or the value is replaced by an existing value having the new key.
    /// \note make sure the the new key does not exist. Otherwise, the element
    /// will be removed if the uniqueness must be ensured
    InsertResult replaceKey(typename ValueType::KeyType const& key,
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
        return {nullptr, false};
    }

    /// \brief delete all entries
    /// if the ValueType is a trivially destructible type and it owns the pool,
    /// this will do a fast internal memory clear without going through all
    /// entries to destroy allocated object. Otherwise a normal clear will be
    /// performed
    void clear()
    {
        if (std::is_trivially_destructible<ValueType>::value && owns_pool_)
        {
            for (auto & b : buckets_) b=nullptr;
            pool_->clear();
        }
        else
        {
            // Note: the content deleted in the pool is untouched so we can still
            // have access to the next item. This assumption is based on single
            // thread access assumption.
            for (auto entry : *this) pool_->del(entry);
            for (auto & b : buckets_) b=nullptr;
        }
    }

  private:

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
        return const_cast<EntryType**>(firstPos());
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
        return const_cast<EntryType**>(nextPos(entry));
    }

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
    allocator_type *                    pool_ {nullptr};
    bool                                owns_pool_ {false};
    size_t                              bucket_number_;
    size_t                              bucket_ix_mask_;
};

}