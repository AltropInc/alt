#pragma once
#include <util/system/Platform.h>
#include <vector>

namespace alt{


/**
 * \class CollectableVector
 * \brief Implements a vector avoiding compact when enries are removed.
 * When an entry is remove, simply set the value to T(0) in the entry.,
 * and put the empty entry index into collector_.
 * On add a new entry, check if there is any empty entry to reuse before
 * calling push_back
 */
template <typename T>
class CollectableVector
{
	std::vector<T> table_;
    std::vector<size_t> collector_;

  public:

	CollectableVector (size_t init_size=128)
	{
        table_.reserve(init_size);
        collector_.reserve(init_size);
	};

    T getEntry(size_t index) noexcept(false)
	{ 
        return table_[index]; // index < table_.size() ? table_[index] : T(NullValue);
    };

	size_t addEntry (const T& entry)
    {
        if (!collector_.empty())
        {
            size_t index = collector_.back();
            table_[index] = entry;
            collector_.pop_back();
            return index;
        }
        table_.push_back(entry);
    }

	void deleteEntry (size_t index)
    {
        if (index < table_.size())
        {
            table_[index] = T(0);
            collector_.push_back(index);
        }
    }

	void setEntry (size_t index, const T& entry)
    {
        if (index >= table_.size())
        {
            table_.resize(index+128, T(0));
        }
        if (!table_[index])
        {
            table_[index] = entry;
        }
    }

    size_t size() const { return table_.size(); }
};

}


