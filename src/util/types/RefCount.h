

#pragma once

#include <util/Defs.h>              // for ALT_UTIL_PUBLIC
#include <atomic>


namespace alt {

/**
 * \struct RefCounter
 * \ingroup Util
 * \brief Implements atomic reference counter.
 */
class RefCounter
{
public:
	RefCounter(): counter_(1) {};

	~RefCounter() {}

	operator int()
    {
        return counter_;
    }

	int operator ++()
    {
        return counter_.fetch_add(1, std::memory_order_relaxed) + 1;
    }

	int operator ++(int)
    {
        return counter_.fetch_add(1, std::memory_order_relaxed);
    }

	int operator --()
    {
        return counter_.fetch_sub(1, std::memory_order_acquire) - 1;
    }

	int operator --(int)
    {
        return counter_.fetch_sub(1, std::memory_order_acquire);
    }

private:
	RefCounter(const RefCounter&) = delete;
	RefCounter(RefCounter&&) = delete;
	RefCounter& operator = (const RefCounter&) = delete;
	RefCounter& operator = (RefCounter&&) = delete;

	mutable std::atomic<int> counter_;
};

/**
 * \class RefCounted
 * \ingroup Util
 * \brief A based for all ref counted objects
 */
class ALT_UTIL_PUBLIC RefCounted
{
public:
	RefCounted() {}

    /// \brief increase ref count on copy
    /// \note this must be called in assignment operator in derived classes
	void onCopy() const { counter_++; }

    /// \brief decrease ref count on release and delete this when the counter is zero
    /// \note instead of calling delete instance of a derived class, calling release
	void release() { if (--counter_ == 0) delete this; }

    /// \return the current ref count value
	int refCount() const { return counter_; }

protected:
	virtual ~RefCounted() {}

private:
	RefCounted(const RefCounted&) = delete;
	RefCounted(RefCounted&&) = delete;
	RefCounted& operator = (const RefCounted&) = delete;
	RefCounted& operator = (RefCounted&&) = delete;

	mutable RefCounter counter_;
};

} // namespace alt

