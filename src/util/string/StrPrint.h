#pragma once

#include "StrBuffer.h"

#include <util/numeric/Intrinsics.h>   // for s_double_digits, s_exp10

#include <stddef.h>
#include <string>
#include <tuple>

namespace alt {

template <typename BufferT>
class StrPrint
{
  public:
    explicit StrPrint(BufferT& buffer) : buffer_(buffer) {}

    const char* c_str() const { return buffer_.c_str(); }

    size_t length() const { return buffer_.length(); }
    bool empty() const { return buffer_.empty(); }
    bool overflowed() const
    { return std::is_same<std::string, BufferT>::value ? false : buffer_.overflowed(); }

    void clear() { buffer_.clear(); }

    void resize(size_t sz) { buffer_.resize(sz); }
   
    // Append char, and string
    StrPrint& operator << (char val) { buffer_.push_back(val); return *this; }
    StrPrint& operator << (const char * val) { buffer_.append(val); return *this; }
    StrPrint& operator << (const std::string& val) { buffer_.append(val); return *this; }

    // Use tuple because operator << can only take a single parameter
    StrPrint& operator << (std::tuple<const char *, size_t> val)
    { buffer_.append(std::get<0>(val), std::get<1>(val)); return *this;  }
    StrPrint& operator << (std::tuple<size_t, char> val)
    { buffer_.append(std::get<0>(val), std::get<1>(val)); return *this; }
    
    StrPrint& operator << (uint32_t);
    StrPrint& operator << (int32_t);
    StrPrint& operator << (uint64_t);
    StrPrint& operator << (int64_t);
    StrPrint& operator << (uint16_t val) { return *this << uint32_t(val); }
    StrPrint& operator << (int16_t val) { return *this << int32_t(val); }
    StrPrint& operator << (uint8_t val) { return *this << uint32_t(val); }
    StrPrint& operator << (int8_t val) { return *this << int32_t(val); }

    StrPrint& operator << (double val);
    StrPrint& operator << (std::tuple<double, int> val);
    StrPrint& operator << (float val) { return *this << double(val); }
    StrPrint& operator << (std::tuple<float, int> val)
     { return *this << std::make_tuple(std::get<0>(val), std::get<1>(val)); }

    // conditionally define methods to print 128-bit integer and double if
    // 128-bit numers are supported
    typename std::enable_if<!std::is_same<ullong, uint64_t>::value, StrPrint>::type&
    operator << (ullong);
    typename std::enable_if<!std::is_same<llong, int64_t>::value, StrPrint>::type&
    operator << (llong);
    typename std::enable_if<!std::is_same<ldouble, double>::value, StrPrint>::type&
    operator << (ldouble val);
    typename std::enable_if<!std::is_same<ldouble, double>::value, StrPrint>::type&
    operator << (std::tuple<ldouble, int> val);

    template <typename T>
    StrPrint& operator << (const T& val) { return *this << val.toString(); }
   
  private:
    BufferT&    buffer_;
};

template <size_t N>
class StrPrinter: public StrPrint<StrBuf>
{
    char        buffer_[N+1];
    StrBuf      str_buffer_;
  public:
    StrPrinter() : StrPrint(str_buffer_), str_buffer_(buffer_, N)
    {
        buffer_[N] = '\0';
    }
    const char* c_str() const { return str_buffer_.c_str(); }
};

template<typename BufferT>
ALT_INLINE StrPrint<BufferT>& StrPrint<BufferT>::operator << (uint32_t val)
{
    if (val < 10)
    {
        buffer_.push_back('0' + val);
    }
    else if (val < 100)
    {
        buffer_.append(s_double_digits + val*2, 2);
    }
    else if (val < 10000)
    {
        unsigned high = val / 100;
        unsigned low = val % 100;
        *this << high;
        if (low <10)
        {
            buffer_.push_back('0' + low);
        }
        *this << low;
    }
    else
    {
        unsigned high = val / 10000;
        unsigned low = val % 10000;
        *this << high;
        if (low <1000) buffer_.push_back('0');
        if (low <100) buffer_.push_back('0');
        if (low <10) buffer_.push_back('0');
        *this << low;
    }
    return *this;
}

template<typename BufferT>
ALT_INLINE StrPrint<BufferT>& StrPrint<BufferT>::operator << (int32_t val)
{
    if (val >= 0)
    {
       return *this << uint32_t(val);
    }
    buffer_.push_back('-');
    return *this << uint32_t(-val);
}

template<typename BufferT>
ALT_INLINE StrPrint<BufferT>& StrPrint<BufferT>::operator << (uint64_t val)
{
    if (val < 10000000)
    {
        return *this << (uint32_t)val;
    }
    uint64_t high = val / 10000000;
    uint32_t low = val % 10000000;
    *this << high;
    if (low <1000000) buffer_.push_back('0');
    if (low <100000) buffer_.push_back('0');
    if (low <10000) buffer_.push_back('0');
    if (low <1000) buffer_.push_back('0');
    if (low <100) buffer_.push_back('0');
    if (low <10) buffer_.push_back('0');
    return *this << low;
}


template<typename BufferT>
ALT_INLINE StrPrint<BufferT>& StrPrint<BufferT>::operator << (int64_t val)
{
    if (val >= 0)
    {
       return *this << uint64_t(val);
    }
    buffer_.push_back('-');
    return *this << uint64_t(-val);
}

template<typename BufferT> ALT_INLINE
typename std::enable_if<!std::is_same<ullong, uint64_t>::value, StrPrint<BufferT>>::type&
StrPrint<BufferT>::operator << (ullong val)
{
    if (val < 100000000000000ULL)
    {
        return *this << (uint64_t)val;
    }
    ullong high = val / 100000000000000ULL;
    uint64_t low = val % 100000000000000ULL;
    *this << high;
    if (low <10000000000000) buffer_.push_back('0');
    if (low <1000000000000) buffer_.push_back('0');
    if (low <100000000000) buffer_.push_back('0');
    if (low <10000000000) buffer_.push_back('0');
    if (low <1000000000) buffer_.push_back('0');
    if (low <100000000) buffer_.push_back('0');
    if (low <10000000) buffer_.push_back('0');
    if (low <1000000) buffer_.push_back('0');
    if (low <100000) buffer_.push_back('0');
    if (low <10000) buffer_.push_back('0');
    if (low <1000) buffer_.push_back('0');
    if (low <100) buffer_.push_back('0');
    if (low <10) buffer_.push_back('0');
    return *this << low;
}

template<typename BufferT> ALT_INLINE
typename std::enable_if<!std::is_same<llong, int64_t>::value, StrPrint<BufferT>>::type&
StrPrint<BufferT>::operator << (llong val)
{
    if (val >= 0)
    {
       return *this << ullong(val);
    }
    buffer_.push_back('-');
    return *this << ullong(-val);
}

template<typename BufferT>
ALT_INLINE StrPrint<BufferT>& StrPrint<BufferT>::operator << (double val)
{
    return *this << std::make_tuple(val,6);
}

template<typename BufferT>
ALT_INLINE StrPrint<BufferT>& StrPrint<BufferT>::operator << (std::tuple<double, int> val)
{
    size_t precision = std::get<1>(val);
    double dval = std::get<0>(val);
    if (dval < 0)
    {
        buffer_.push_back('-');
        dval = -dval;
    }
    uint64_t exp = s_exp10[precision];
    int64_t lval = int64_t(dval * exp + 0.5);
    *this << int64_t(lval / exp) << '.';
    int64_t fval = lval % exp;
    for (int p = precision - 1; p > 0 && fval < s_exp10[p]; --p)
    {
       buffer_.push_back('0');
    }
    return *this << fval;
}

template<typename BufferT> ALT_INLINE
typename std::enable_if<!std::is_same<double, ldouble>::value, StrPrint<BufferT>>::type&
StrPrint<BufferT>::operator << (ldouble val)
{
    return *this << std::make_tuple(val,12);
}

template<typename BufferT> ALT_INLINE
typename std::enable_if<!std::is_same<double, ldouble>::value, StrPrint<BufferT>>::type&
StrPrint<BufferT>::operator << (std::tuple<ldouble, int> val)
{
    size_t precision = std::get<1>(val);
    ldouble dval = std::get<0>(val);
    if (dval < 0)
    {
        buffer_.push_back('-');
        dval = -dval;
    }
    ullong exp = s_exp10[precision];
    llong lval = llong(dval * exp + 0.5);
    *this << llong(lval / exp) << '.';
    llong fval = lval % exp;
    for (int p = precision - 1; p > 0 && fval < s_exp10[p]; --p)
    {
       buffer_.push_back('0');
    }
    return *this << fval;
}
}
