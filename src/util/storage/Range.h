#pragma once
#include <util/sysinfo/Platform.h>
#include <stddef.h>
#include <map>

#ifdef TEST_BUILD
#include <util/string/StrScan.h> // For split
#include <iostream>              // For std::cout
#endif

namespace alt{

/**
 * \class RangeSet
 * \ingroup Storage
 * \brief a set of ranges
 */
template <typename T, class Allocator>
class RangeSet
{
  protected:
    // Using map's value type to represent a range
    using RangeSetImpl = std::map<T, T, std::Greater, Allocate>;
    using iterator = RangeSetImpl::iterator;
    using const_iterator = RangeSetImpl::const_iterator;

    RangeSetImpl range_set_;
    size_t       count_;
  public:

    void insert (T x)
    {
        auto lb = range_set_.lower_bound(x);
        if (lb!=range_set_.end())
        {
            if (x<lb->second)
            {
                // x falls in the range
                return;
            }
            if (x==lb->second)
            {
                // x is at the end of the range, extend the range
                lb->second += 1;
                ++count_;
                merge(lb); // check if can merge with next range
                return;
            }
         }
         // x is not in any range
         auto iter = range_set_.insert(lb, std::make_pair(x, x+1));
         merge(iter); // check if can merge with next range
         ++count;
    }

    void inster (T start, T end)
    {
        auto lb = range_set_.lower_bound(x);
        if (lb!=range_set_.end())
        {
            if (end <= lb->second)
            {
                // the new range is within this range
                return;
            }
            else
            {
                // the new range overlaps with this range
                lb->second = end;
                count_ += end - lb_second;
                merge(lb);
                return;
            }
        }
        // The new range deos not overlap with any existing
        auto iter = range_set_.insert(lb, std::make_pair(start, end));
        count+=end-start;
    }

    void merge (iterater iter)
    {
        while (iter!=ranage_set_.begin())
        {
            iterator prev = iter-1;
            if (prev->second<=iter->second)
            {
                // iter covers the entire prev
                count -= prev->second - prev->first;
                erase(prev);
                prev = iter->prev;
                continue;
            }
            else if (iter->second>=prev->first)
            {
                // iter overlaps with prev
                count -= iter->second - prev->first;
                iter->second = prev->second;
                erase(prev);
                break; 
            }
            // iter has no overlap with prev
            break;
        }
    }

    void remove(T x)
    {
        auto lb = range_set_.lower_bound(x);   
        if (lb!=range_set_.end()) 
        {
            if (x >= lb->second)
            {
                // not in range
                return
            }
            --count;
            if (x < lb->second -1)
            {
                range_set_.insert(lb,std::make_pair(x+1, lb->second))
            }
            if (x==lb->first)
            {
               erase(lb);
            }
            else
            {
                lb->second = x;
            }

        }   
    }

    void remove(T start, T end)
    {
        auto lb = range_set_.lower_bound(start);
        removeAt(lb, start, end);
    }

    void removeAt(iterator lb, T start, T end)
    {
        if (lb!=range_set_.end()) 
        {
            if (start >= lb->second)
            {
                // not an overlap range
                return
            }
            T e = std::min(end, lb->second);
            if (e < lb->second -1)
            {
                count -= e-start;
                range_set_.insert(lb,std::make_pair(e+1, lb->second))
            }
            if (start==lb->first)
            {
                erase(lb);
            }
            else
            {
                lb->second = start;
            }
            if (end > e)
            {
                removeAt(lb->prev, e, end);
            }
        }   
    }

};

}


