#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file SideBuckets.h
 * @library alt_util
 * @brief definition of a sorted buckets to store values that change frequently at the
 * top side in an indexed buffer (the front bucket buffer) so the search of frequently
 * used buckets will be O(1). Buckets with values far away from the top beyond limit of
 * the front bucket buffer will go into the back bucket buffer, which is a SortedBuckets
 * and the cost of seach of infrequently used buckets will be O(logN).
 */

#include "SortedBuckets.h"

namespace alt{

/**
 * \class SideBuckets
 * \ingroup StorageUtils
 * \brief defines sorted buckets, where the front bucket buffer is a ring buffer to
 * store the active side with an indexed hash. Values far away from the top beyond
 * limit of the front bucket buffer are stored in a SortedBuckets.
 */
template <typename Key, typename T, class Compare = SortedBucketCompareInc<Key>>
class SideBuckets
{
  public:

    using value_type = std::pair<Key, T>;

    void reset()
    {
        // set top_ix at the 1/4th position in front_bucks_
        top_ix_ = front_bucket_sz_ >> 2;
        bot_ix_ = top_ix_;
        top_ = Compare::max();
        back_bucks_.reset();
    }

    explicit SideBuckets(size_t front_bucket_sz, size_t back_bucket_sz)
        : back_bucks_(back_bucket_sz)
    {
        front_bucket_sz_ = power2Next<size_t>(front_bucket_sz);
        front_bucket_mask_ = front_bucket_sz_ -1;
        front_bucks_.resize(front_bucket_sz_);
        top_ix_ = front_bucket_sz_ >> 2; // set top_ix at the 1/4th position
        bot_ix_ = top_ix_;
    }

    class iterator
    {
      private:
        SideBuckets&  parent_;
        size_t        ix_;
        bool          in_front_ { true };
      public:
        iterator(const iterator& oth):
            parent_(oth.parent_),
            ix_(oth.ix_),
            in_front_(oth.in_front_)
            {}

        iterator(SideBuckets & p, size_t ix, bool in_front):
            parent_(p), ix_(ix), in_front_(in_front) {}

        bool operator==(const iterator& oth)
        {
            return parent_==oth.parent_ && ix_==oth.ix_ &&
                   in_front_==oth.in_front_;
        }

        bool operator!=(const iterator& oth)
        {
            return !(*this==oth);
        }

        iterator& operator=(const iterator&oth)
        {
            parent_=oth.parent_;
            ix_=oth.ix_;
            in_front_==oth.in_front_;
            return *this;
        }

        iterator& operator++()
        {
            if (in_front_)
            {
                if (ix_ < parent_.bot_ix_)
                {
                    ++ix_;
                }
                if (ix_ >= parent_.bot_ix_)
                {
                    if (!parent_.back_bucks_.empty())
                    {
                        ix_= parent_.back_bucks_.head();
                        in_front_ = false;
                    }
                }
                return *this;
            }

            if (ix_<parent_.back_bucks_.tail()) ++ix_;

            return *this;
        }

        iterator& nextNonEmpty()
        {
            if (in_front_)
            {
                if (ix_ < parent_.bot_ix_)
                {
                    ++ix_;
                    while (parent_.isFrontEntryEmpty(ix_))
                    {
                        ++ix_;
                        if (ix_ >= parent_.bot_ix_)
                        {
                            break;
                        }
                    }
                }
                if (ix_ == parent_.bot_ix_)
                {
                    if (!back_bucks_.empty())
                    {
                        ix_= parent_.back_bucks_.head();
                        in_front_ = false;
                    }
                }
                return *this;
            }
            if (ix_<parent_.back_bucks_.tail()) ++ix_;
            return *this;
        }

        value_type operator*() const
        {
            return in_front_ ? parent_.getFrontEntry(ix_)
                             : parent_.getBackEntry(ix_);
        }
    };

    void print()
    {
        std::cout << "FRONT (top_ix_=" <<top_ix_ << " bot_ix_=" << bot_ix_ << "):" << std::endl;
        for (int ix=top_ix_; ix < bot_ix_; ++ix)
        {
            std::cout << '[' << ix << "]: (" << top_ + Compare::diff(ix, top_ix_)
            << ',' <<  front_bucks_[ix & front_bucket_mask_].toStr() << ')' << std::endl;
        }
        std::cout << "BACK: head_=" << back_bucks_.head() << " tail=" << back_bucks_.tail() << "):" << std::endl;
        back_bucks_.print();
    }

    bool frontEmpty() const { return top_ix_==bot_ix_; }
    bool empty() const { return frontEmpty() && back_bucks_.empty(); }
    size_t size() const { return count_ + back_bucks_.size(); }
    iterator begin() 
    {
        if (frontEmpty())
        {
            if (!back_bucks_.empty())
            {
                return iterator(*this, back_bucks_.head(), false);
            }
            return iterator(*this, bot_ix_, true);
        }
        return iterator(*this, top_ix_, true);

    }
    iterator end() 
    {
        if (back_bucks_.empty())
        {
            return iterator(*this, bot_ix_, true);
        }
        return iterator(*this, back_bucks_.tail(), false);
    }

    T* find(const Key& key)
    {
        int key_ix = top_ix_ + Compare::diff(key, top_);
        if (key_ix >= top_ix_ && key_ix < bot_ix_)
        {
            auto& entry = front_bucks_[key_ix & front_bucket_mask_];
            return entry.empty() ? nullptr : &entry;
        }
        return back_bucks_.find(key);
    }

    value_type getFrontEntry(int ix)
    {
        return
            std::make_pair(top_ + Compare::diff(ix, top_ix_), front_bucks_[ix & front_bucket_mask_]);
    }

    value_type getBackEntry(int ix)
    {
        return back_bucks_[ix];
    }

    bool isFrontEntryEmpty(int ix)
    {
        return front_bucks_[ix & front_bucket_mask_].empty();
    }

    T* add (const Key& key, const T& val)
    {
        if (top_ix_ == bot_ix_)
        {
            popToFront();
        }
        if (top_ix_ == bot_ix_)
        {
            resetTop(key, val);
            return &front_bucks_[top_ix_];
        }
        int key_ix = top_ix_ + Compare::diff(key, top_);
        if (key_ix < top_ix_)
        {
            if (!val.isPositive())
            {
                // Do not allow negative values
                return nullptr;
            }
            // new top
            int span = bot_ix_ - key_ix;
            // std::cout << "span=" << span << std::endl;
            if (span > front_bucket_sz_)
            {
                // the span betweew the new top and bot is greater than the front_bucks_ size,
                // push some bottom entries to the back buckets
                pushToBack(span-front_bucket_sz_);
                // if front_bucks_ becomes empty after push, reset top
                if (top_ix_ == bot_ix_)
                {
                    resetTop(key, val);
                    return &front_bucks_[top_ix_];;
                }
            }

            if (key_ix < 0)
            {
                // if key_ix < 0, bot_ix_ must be < front_bucket_sz_
                key_ix += front_bucket_sz_;
                bot_ix_ += front_bucket_sz_;
            }

            front_bucks_[key_ix].add(val);
            top_ = key;
            top_ix_ = key_ix;

            return &front_bucks_[key_ix];
        }

        if (key_ix == top_ix_)
        {
            front_bucks_[top_ix_].add(val);
            if (front_bucks_[top_ix_].empty())
            {
                adjustTop(top_ix_);
            }
            return &front_bucks_[top_ix_];
        }

        int span = key_ix - top_ix_;
        if (span < front_bucket_sz_)
        {
            if (key_ix >= bot_ix_)
            {
                // check if some entries in back need to move to front before we add
                // the entry beyond the bottom
                popToFront();
            }

            front_bucks_[key_ix & front_bucket_mask_].add(val);

            if (key_ix >= bot_ix_)
            {
                bot_ix_ = key_ix + 1;
            }

            if (key_ix == bot_ix_ -1)
            {
                if (front_bucks_[key_ix & front_bucket_mask_].empty())
                {
                    adjustBot();
                }
            }

            return &front_bucks_[key_ix & front_bucket_mask_];
        }

        return back_bucks_.add(key, val);
    }

    T* update (const Key& key, const T& val)
    {
        if (top_ix_ == bot_ix_)
        {
            return back_bucks_.update(key, val);
        }
        int key_ix = top_ix_ + Compare::diff(key, top_);
        if (key_ix < top_ix_)
        {
            return nullptr;
        }
        if (key_ix < bot_ix_)
        {
            T& existing = front_bucks_[key_ix & front_bucket_mask_];
            if (!existing.empty())
            {
                existing.update(val);
                if (key_ix==top_ix_)
                {
                    if (front_bucks_[top_ix_].empty())
                    {
                        adjustTop(top_ix_);
                        return nullptr;
                    }
                    return &front_bucks_[top_ix_];              
                }
                if (key_ix==bot_ix_-1)
                {
                    if (front_bucks_[key_ix & front_bucket_mask_].empty())
                    {
                        adjustBot();
                        return nullptr;
                    }
                }
                return &front_bucks_[key_ix & front_bucket_mask_];
            }
            return nullptr;
        }
        back_bucks_.update(key, val);
    }

  protected:

    void resetTop(const Key& key, const T& val)
    {
        // set top_ix at the 1/4th position in front_bucks_
        top_ix_ = front_bucket_sz_ >> 2;
        bot_ix_ = top_ix_+1;
        front_bucks_[top_ix_].add(val);
        top_ = key;
    }

    // Pop entries in back_bucks to front_bucks
    void popToFront ()
    {
        while (!back_bucks_.empty())
        {
            const auto& en = back_bucks_.front();
            if (top_ix_ == bot_ix_)
            {
                resetTop(en.first, en.second);
                back_bucks_.pop_front();
                continue;
            }
            int diff =  Compare::diff(en.first, top_);
            if (diff >= front_bucket_sz_)
            {
                break;
            }
            int key_ix = top_ix_ + diff;
            front_bucks_[key_ix & front_bucket_mask_].add( en.second);
            bot_ix_ = key_ix + 1;
            back_bucks_.pop_front();
        }
    }

    /// Push at least num entries from front to back
    void pushToBack (int num)
    {
        //std::cout << "pushToBack num=" << num << std::endl;
        while (num > 0 && bot_ix_ > top_ix_)
        {
            --bot_ix_;
            --num;
            T* bot_val = &front_bucks_[bot_ix_ & front_bucket_mask_];
            while (bot_val->empty() && num>0)
            {
                --bot_ix_;
                --num;
                bot_val = &front_bucks_[bot_ix_ & front_bucket_mask_];
            }
            if (num >= 0 && !bot_val->empty())
            {
                Key bot = top_ + Compare::diff(bot_ix_, top_ix_);
                //std::cout << "back_bucks_.push_front " << bot << " " << bot_val->toStr() << std::endl;
                back_bucks_.push_front(bot, *bot_val);
                bot_val->reset();
            }
        }
    }

    void adjustTop(int old_top)
    {
        ++top_ix_;
        while (top_ix_ < bot_ix_ && front_bucks_[top_ix_ & front_bucket_mask_].empty())
        {
            ++top_ix_;
        }
        top_ = top_ + Compare::diff(top_ix_, old_top);

        // adjust top_ix_ to make it within the range [0, front_bucket_sz_)
        if (top_ix_ >= front_bucket_sz_)
        {
            top_ix_ -= front_bucket_sz_;
            bot_ix_ -= front_bucket_sz_;
        }
    }

    void adjustBot()
    {
        --bot_ix_;
        while (bot_ix_ > top_ix_ && front_bucks_[(bot_ix_-1) & front_bucket_mask_].empty())
        {
            --bot_ix_;
        }
    }
    
  private:
    std::vector<T>                  front_bucks_;
    SortedBuckets<Key, T, Compare>  back_bucks_;

    int     top_ix_ {0};           ///< index of the top entry in front_bucks_
    int     bot_ix_ {0};           ///< index of the bottom entry in front_bucks_
    Key     top_ {Compare::max()}; ///< the top key value
    size_t  count_ {0};            ///< number of non-empty entries in front_bucks_

    size_t  front_bucket_sz_;      ///< size of front_bucks_
    size_t  front_bucket_mask_;    ///< module mask of front_bucks_ bot_ix_

};

} // namesapce alt

