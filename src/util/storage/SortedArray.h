#pragma once
#include <util/sysinfo/Platform.h>
#include <util/numeric/Intrinsics.h> // For isel, power2Next
#include <stddef.h>
#include <vector>
#include <functional>
#include <cstring>

// for TEST_BUILD
#include <iostream>         // For std::cout

namespace alt{

/**
 * \class SortedArray
 * \ingroup StorageUtils
 * \brief Sorted array
 */
template <typename T, class Compare=std::less<T> >
class SortedArray
{
  protected:

    std::vector<T>   array_;
    size_t head_;
    size_t tail_;

    size_t lowBound(const T& x)
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

  public:
    SortedArray(size_t capacity)
        : array_(capacity)
        , head_ {capacity/2}
        , tail_ {capacity/2}
    {}

    void insert (const T& x)
    {
        if (tail_ - head_ >= array_.size())
        {
            size_t new_size = array_.size()*2;
            std::cout<<"RESIZE TO "<< new_size <<std::endl;
            array_.resize(new_size);
            print();
        }
        size_t ix = lowBound(x);
        std::cout << "LB=" << ix << " T=" << tail_ << " H=" << head_;
        if (ix==tail_ && tail_ < array_.size())
        {
            std::cout << " A" << std::endl;
            array_[tail_++] = x;
        }
        else if (ix==head_ && head_>0)
        {
            std::cout << " B" << std::endl;
            array_[--head_] = x;
        }
        else if (ix - head_ < tail_ - ix && head_ > 0 || tail_>=array_.size())
        {
            std::cout << " C" << std::endl;
            std::memmove(&array_[head_-1], &array_[head_], sizeof(T)*(ix-head_));
            --head_;
            array_[ix-1] = x;
        }
        else
        {
            std::cout << " D" << std::endl;
            std::memmove(&array_[ix+1], &array_[ix], sizeof(T)*(tail_-ix));
            ++tail_;
            array_[ix] = x;
        } 
        print();   
    }

    void print()
    {
        for (size_t i=head_; i<tail_; ++i)
            std::cout << '[' << i << "]: " << array_[i] << std::endl;
    }
};

} // namesapce alt

