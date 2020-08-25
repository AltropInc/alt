#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file SortedBuckets.h
 * @library alt_util
 * @brief definition of a sorted buckets in which the bucket value can
 * be merged (accumulated or deducted). If the bucket value becomes zero,
 * it will be removed. Buckets starts in the middle of the storage and then
 * expand on either direction depending on which end is shorter for the memory
 * movement. This significantly reduce the need to move memory if inserting/
 * deleting buckets mostly happens at the end or near the end of the bucket
 * storage., for instance, in a price book.
 */

#include <util/system/Platform.h>
#include <util/numeric/Intrinsics.h> // For isel, power2Next
#include <stddef.h>
#include <vector>
#include <functional>
#include <cstring>
#include <type_traits>               // for is_trivially_destructible

// for TEST_BUILD
#include <iostream>         // For std::cout

namespace alt {

template <typename Key>
struct SortedBucketCompareInc
{
    static int threeway (Key x, Key y)
    {
        return x==y ? 0 : x<y ? 1 : -1;
    }
    static Key max() { return std::numeric_limits<Key>::max(); }
    template <typename T>
    static T diff(T x, T y) { return x-y; }
};

template <typename Key>
struct SortedBucketCompareDec
{
    static int threeway (Key x, Key y)
    {
        return x==y ? 0 : x<y ? -1 : 1;
    }
    static Key max() { return std::numeric_limits<Key>::min(); }
    template <typename T>
    static T diff(T x, T y) { return y-x; }
};

/**
 * \class SortedBuckets
 * \ingroup StorageUtils
 * \brief Sorted buckets. Each bucket contains a key and value in type of T.
 * \tparam Key the key type used in search, must be trivially destructible
 * \tparam T the value type, must be trivially destructible
 * \tparam Compare the function for the order based on the key value.
 * \note Buckets will be kept in the middle of the container in order to reduce
 * the chance of memory move, especially when buckets are frequently added and
 * removed at either ends.
 */
template <typename Key, typename T, class Compare = SortedBucketCompareInc<Key>>
class SortedBuckets
{
  public:
    using value_type = std::pair<Key, T>;
    using reference = value_type&;
    using pointer = value_type*;
  protected:

    std::vector<value_type>   buckets_;
    size_t head_;
    size_t tail_;
    static_assert(std::is_trivially_destructible<Key>::value);
    static_assert(std::is_trivially_destructible<T>::value);

    size_t lowBound(const Key& x)
    {
        size_t start = head_;
        size_t end = tail_;
        size_t mid;
        while (start < end)
        {
            mid = (start+end)/2;
            int res = Compare::threeway(x, buckets_[mid].first);
            if (res > 0)
            {
                end = mid;
            }
            else if (x==buckets_[mid].first)
            {
                return mid;
            }
            else
            {
                start = mid+1;
            } 
        }
        return end;
    }

    void erase (size_t ix)
    {
        //  Compact memory from shorter end.
        if (ix - head_ < tail_ - ix)
        {
            if (ix>head_)
            {
                std::memmove(&buckets_[head_+1], &buckets_[head_], sizeof(value_type)*(ix-head_));
            }
            ++head_;
        }
        else
        {
            if (tail_ > ix)
            {
                std::memmove(&buckets_[ix], &buckets_[ix+1], sizeof(value_type)*(tail_-ix));
            }
            --tail_;
        } 
    }

  public:
    SortedBuckets(size_t capacity)
        : buckets_(capacity)
        , head_ {capacity/2}
        , tail_ {capacity/2}
    {}

    void reset()
    {
        head_ = buckets_.size()/2;
        tail_ = head_;
    }

    class iterator
    {
        SortedBuckets& parent_;
        size_t        ix_;
      public:
        iterator(const iterator& oth): parent_(oth.parent_), ix_(oth.ix_) {}
        iterator(SortedBuckets & p, size_t ix): parent_(p), ix_(ix) {}
        bool operator==(const iterator& oth) { return ix_==oth.ix_; }
        iterator& operator=(const iterator&oth) { ix_=oth.ix_; return *this; }
        iterator& operator++() { if (ix_<parent_.tail_) ++ix_; return *this; }
        iterator& operator--() { if (ix_>parent_.head_) --ix_; else ix_=parent_.tail_; return *this; }
        reference operator*() const { return parent_.buckets_[ix_]; }
        pointer operator->() const { return &(parent_.buckets_[ix_]); }
        size_t index() const { return ix_; }
        friend void swap(iterator& lhs, iterator& rhs) { std::swap(lhs.ix_, rhs.ix_); }
    };

    iterator begin() { return iterator(*this, head_); }
    iterator end() { return iterator(*this, tail_); }
    iterator last() { return iterator(*this, tail_==head_?tail_:tail_-1); }

    T* find(const Key& key)
    {
        size_t ix = lowBound(key);
        return (ix < tail_ && buckets_[ix].first == key)
                ? &(buckets_[ix].second) : nullptr;
    }

    void pop_front () { if (head_ < tail_) ++head_; }
    value_type& front () { return buckets_[head_]; }
    const value_type& front () const { return buckets_[head_]; }
    void push_front (const Key& key, const T& val)
    {
        if (head_ == 0)
        {
            if (tail_ >= buckets_.size()-1)
            {
                 buckets_.resize(buckets_.size()*2);
            }
            size_t dist = std::min(size_t(1), (buckets_.size() - tail_ + 1) / 2);
            std::memmove(&buckets_[head_+dist], &buckets_[head_], sizeof(value_type)*(tail_-head_));
            head_ += dist;
            tail_ += dist;
        }
        buckets_[--head_] = std::make_pair(key, val);
    }
 
    void pop_back () { if (head_ < tail_) --tail_; }
    value_type& back () { return buckets_[tail_-1]; }
    const value_type& back () const { return buckets_[tail_-1]; }
    const value_type& operator[] (size_t ix) const { return buckets_[ix]; }

    size_t size() const { return tail_ - head_; }
    bool empty() const { return tail_ == head_; }
    size_t head() const { return head_; }
    size_t tail() const { return tail_; }

    T* update (const Key& key, const T& val)
    {
        size_t ix = lowBound(key);
        if (ix < tail_ && buckets_[ix].first == key)
        {
            // update the given part in val from existing
            buckets_[ix].second.update(val);
            if (buckets_[ix].second.empty())
            {
                erase(ix);
                return nullptr;
            }
            return &(buckets_[ix].second);
        }
        return nullptr;
    }

    T* add (const Key& key, const T& val)
    {
         size_t ix = lowBound(key);
        // std::cout << "ix=" << ix << " head=" << head_ << " tail=" << tail_ << std::endl;
        if (ix < tail_ && buckets_[ix].first == key)
        {
            // Find the bucket in equal value. Add the value into the bucket
            buckets_[ix].second.add(val);
            if (buckets_[ix].second.empty())
            {
                erase(ix);
                return nullptr;
            }
            return &buckets_[ix].second;
        }

        if (val.empty())
        {
            // do not enter empty entry
            return nullptr;
        }

        if (ix==tail_ && tail_ < buckets_.size())
        {
            buckets_[tail_++] = std::make_pair(key, val);
            return &buckets_[ix].second;
        }

        if (ix==head_ && head_>0)
        {
            buckets_[--head_] = std::make_pair(key, val);
            return &buckets_[head_].second;
        }

        if (tail_ - head_ >= buckets_.size())
        {
            buckets_.resize(buckets_.size()*2);
            // std::cout << "SortedBuckets resize to " << buckets_.size() << std::endl;
        }

        if (ix - head_ < tail_ - ix && head_ > 0 || tail_>=buckets_.size())
        {
            std::memmove(&buckets_[head_-1], &buckets_[head_], sizeof(value_type)*(ix-head_));
            --head_;
            buckets_[ix-1] = std::make_pair(key, val);
            return &buckets_[ix-1].second;
        }

        std::memmove(&buckets_[ix+1], &buckets_[ix], sizeof(value_type)*(tail_-ix));
        ++tail_;
        buckets_[ix] = std::make_pair(key, val);
        return &buckets_[ix].second;
    }

    void print()
    {
        for (size_t i=head_; i<tail_; ++i)
            std::cout << '[' << i << "]: (" << buckets_[i].first
            << ',' <<  buckets_[i].second.toStr() << ')' << std::endl;
    }
};

} // namespace alt

