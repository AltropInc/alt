#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file SortedArray.h
 * @library alt_util
 * @brief definition of a sorted array. Inserted values start in the
 * middle of the storage and then expand on either direction depending
 * on which end is shorter for the memory movement.
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

namespace alt{

/**
 * \class SortedArray
 * \ingroup StorageUtils
 * \brief Sorted array
 * \tparam T the value type, must be trivially destructible, because this array uses
 * fast memory move
 * \tparam Compare function for the order based on the key value.
 */
template <typename T, class Compare=std::less<T> >
class SortedArray
{
  public:

    typedef T                   value_type;
    typedef T&                  reference;
    typedef const T&            const_reference;
    typedef T*                  pointer;
    typedef const T*            const_pointer;

    template <typename RefT, typename PtrT>
    class IteratorT
    {
        SortedArray& parent_;
        size_t        ix_;
      public:
        IteratorT(const IteratorT& oth): parent_(oth.parent_), ix_(oth.ix_) {}
        IteratorT(SortedArray & p, size_t ix): parent_(p), ix_(ix) {}
        bool operator==(const IteratorT& oth) { return ix_==oth.ix_; }
        bool operator!=(const IteratorT& oth) { return ix_!=oth.ix_; }
        IteratorT operator=(const IteratorT&oth) { ix_=oth.ix_; return *this; }
        IteratorT operator++() { if (ix_<parent_.tail_) ++ix_; return *this; }
        IteratorT operator--() { if (ix_>parent_.head_) --ix_; else ix_=parent_.tail_; return *this; }
        RefT operator*() const { return parent_.array_[ix_]; }
        PtrT operator->() const { return &(parent_.array_[ix_]); }
        size_t index() const { return ix_; }
    };

    using iterator = IteratorT<reference, pointer>;
    using const_iterator = IteratorT<const_reference, const_pointer>;

    /// \brief constructs an array with an initial capacity
    SortedArray(size_t capacity)
        : array_(capacity)
        , head_ {capacity/2}
        , tail_ {capacity/2}
    {}

    /// \brief constructs an array initially empty and will start an initial
    /// capacity of 16 when insert is called
    SortedArray()
        : head_ {0}
        , tail_ {0}
    {}
    
    iterator begin() { return iterator(*this, head_); }
    iterator end() { return iterator(*this, tail_); }
    iterator last() { return iterator(*this, tail_==head_?tail_:tail_-1); }

    const_iterator begin() const { return iterator(*this, head_); }
    const_iterator end() const{ return iterator(*this, tail_); }
    const_iterator last() const { return iterator(*this, tail_==head_?tail_:tail_-1); }

    size_t size() const { return tail_-head_; }
    const_reference operator[] (size_t ix) const { return array_[ix+head_]; }
    const_reference operator[] (size_t ix) { return array_[ix+head_]; }

    /// \brief finds a value in the array
    /// \param x the reference to the value to search
    /// \return the index where the value is found; -1 if the value is not found
    int find(const_reference x)
    {
        size_t ix = lowBound(x);
        if (ix > head_ && array_[ix-1]==x)
        {
            return ix-1;
        }
        if (ix < tail_ && array_[ix]==x)
        {
            return ix;
        }
        return -1;
    }

    /// \brief erase all entries equal to the given value from the array
    /// \param x the reference to the value to be erased
    int erase (const_reference x)
    {
        int ix = find(x);
        if (ix >=0)
        {
            int erase_count = 1;
            while (ix > head_ && array_[ix-1]==x)
            {
                // check to erase all alues equal to x
                ++erase_count;
                --ix;
            }
            // Compact memory from the shorter end.
            if (ix - head_ < tail_ - ix)
            {
                if (ix>head_)
                {
                    std::memmove(&array_[head_+erase_count], &array_[head_], sizeof(value_type)*(ix-head_));
                }
                head_ += erase_count;
            }
            else
            {
                if (tail_ > ix)
                {
                    std::memmove(&array_[ix], &array_[ix+erase_count], sizeof(value_type)*(tail_-ix));
                }
                tail_ -= erase_count;
            }
            return ix - head_;
        }
        return -1;
    }

    /// \brief inserts a value into the array
    /// \param x the reference to the value to be inserted
    /// \param unique if true the value will not be inserted if it already exists
    bool insert (const_reference x, bool unique=false)
    {
        if (tail_ - head_ >= array_.size())
        {
            if (array_.empty())
            {
                array_.resize(16);
                head_ = tail_ = 8;
            }
            else
            {
                size_t new_size = array_.size()*2;
                // std::cout<<"RESIZE TO "<< new_size <<std::endl;
                array_.resize(new_size);
            }
            //print();
        }
        size_t ix = lowBound(x);
        if (unique)
        {
            if ((ix>head_ && array_[ix-1]==x) || (ix < tail_ && array_[ix]==x))
            {
                return false;
            }
        }
        // std::cout << "insert: " << x << " LB=" << ix << " T=" << tail_ << " H=" << head_ << std::endl;
        if (ix==tail_ && tail_ < array_.size())
        {
            //std::cout << " A" << std::endl;
            array_[tail_++] = x;
        }
        else if (ix==head_ && head_>0)
        {
            //std::cout << " B" << std::endl;
            array_[--head_] = x;
        }
        else if (ix - head_ < tail_ - ix && head_ > 0 || tail_>=array_.size())
        {
            //std::cout << " C" << std::endl;
            std::memmove(&array_[head_-1], &array_[head_], sizeof(T)*(ix-head_));
            --head_;
            array_[ix-1] = x;
        }
        else
        {
            //std::cout << " D" << std::endl;
            std::memmove(&array_[ix+1], &array_[ix], sizeof(T)*(tail_-ix));
            ++tail_;
            array_[ix] = x;
        } 
        //print();
         return true;
    }

    int find (T&& x) { return find(x); }

    void insert (T&& x, bool unique=false) { insert(x, unique); }

    bool erase (T&& x) { return erase(x); }

    void print()
    {
        for (size_t i=head_; i<tail_; ++i)
            std::cout << '[' << i << "]: " << array_[i] << std::endl;
    }

    void clear()
    {
        array_.clear();
        tail_ = head_ = array_.capacity()/2;
    }

  protected:
    std::vector<T>   array_;
    size_t head_;
    size_t tail_;
    static_assert(std::is_trivially_destructible<T>::value);

    size_t lowBound(const_reference x)
    {
        size_t start = head_;
        size_t end = tail_;
        size_t mid;
        while (start < end)
        {
            mid = (start+end)/2;
            if (Compare()(x, array_[mid]))
            {
                end = mid;
            }
            else
            {
                start = mid+1;
            } 
        }
        return end;
    }
};

} // namespace alt

