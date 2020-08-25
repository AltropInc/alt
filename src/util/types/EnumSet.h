#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file EnumSet.h
 * @library alt_util
 * @brief Defines reflective Enum Set Type that supports that works for both
 * reflective enum adn standard C++ enum. Howver, the reflective feature for
 * standard C++ enum is not available
 */

#include "Enum.h"

#include <util/string/StrUtils.h>
#include <bitset>
#include <iostream>

namespace alt
{

/**
 * \struct EnumToSizeT
 * \ingroup Types
 * \brief helper to convert standard enum or refelective enum value to size_t
 */
struct EnumToSizeT
{
    template <class ET>
    typename std::enable_if<std::is_enum<ET>::value,size_t>::type
    static get(ET e)
    {
        return static_cast<typename std::underlying_type<ET>::type>(e);
    }

    template <class ET>
    typename std::enable_if<!std::is_enum<ET>::value,size_t>::type
    static get(ET e)
    { 
        return ET::toUnderlying(e);
    }
};

/**
 * \struct BitSet
 * \ingroup Types
 * \brief bitset template on an integral underlying type as an alternative for
 * std::bitset used to construct EnumSet. See EnumSet. This is used whe you need a
 * fixed-sized value.
 * \tparam T underlying type for the bit set type.
 */
template <typename T>
struct BitSet
{
    static_assert(std::is_integral<T>::value, "Integral required for BitSet");
    T   value_ { 0UL} ;
    BitSet() = default;
    BitSet(const BitSet& oth): value_(oth.value_) {}
    BitSet(T val): value_(val) {}
    BitSet& set (size_t v) { value_ |= (T(1) << v); return *this; }
    BitSet& set () { value_ = ~(0UL); return *this; }
    BitSet& reset (size_t v) { value_ &= ~(T(1) << v); return *this; }
    BitSet& reset () { value_ = 0UL;  return *this; }
    bool test (size_t v) const { return (value_ & (T(1) << v))!=0; }
    BitSet& flip (size_t v) { value_ = value_ ^ (T(1) << v); return *this; }
    BitSet& flip () { value_ = ~value_; return *this; }
    friend BitSet operator~ (BitSet bs) { bs.flip(); return bs; }
    bool none() const { return value_==0; }
    bool any() const { return value_!=0; }
    size_t count() const { return __builtin_popcountl(value_); }
    size_t size() const { return __builtin_popcountl(value_); }
    bool operator==(const BitSet &es) const { return value_ == es.value_; }
    bool operator!=(const BitSet &es) const { return value_ != es.value_; }
    BitSet& operator |= (BitSet oth) { value_|= oth.value_; return *this; }
    BitSet& operator &= (BitSet oth) { value_&= oth.value_; return *this; }
    BitSet& operator -= (BitSet oth) { value_&= ~oth.value_; return *this; }
    BitSet& operator += (BitSet oth) { value_|= oth.value_; return *this; }
    BitSet operator | (BitSet oth) const { oth.value_|= value_; return oth; }
    BitSet operator & (BitSet oth) const { oth.value_&= value_; return oth; }
    BitSet operator - (BitSet oth) const { oth.value_&= ~value_; return oth; }
    BitSet operator + (BitSet oth) const { oth.value_|= value_; return oth; }
    uint64_t to_ullong() const { return uint64_t(value_); }
    T toUnderlying() const { return value_; }
    static BitSet fromUnderlying(T val) { return BitSet(val); }

    std::string to_string () const
    {
        std::string output;
        for (size_t e = 0; e < sizeof(T)*8; ++e)
        {
            output.push_back(((T(1) << e) & value_) ? '1' : '0');
        }
        return output;
    }
};

/**
 * \class EnumSet
 * \brief implements an enum set type
 * @note The limitation of not being constexpr for constructor comes from
 *       std::bitset whose constructor is not constexpr as it should be.
 */
template<typename ET, typename BT=std::bitset<sizeof(ET)*CHAR_BIT>>
class EnumSet
{
    static_assert(std::is_enum<ET>::value || std::is_base_of<EnumBase, ET>::value,
        "Enum type required for EnumSet");

    using bitset_t = BT;
    bitset_t bitset_;

  public:
    EnumSet() = default;
    explicit EnumSet(ET e) { bitset_.set(EnumToSizeT::get<ET>(e)); }
    EnumSet(const EnumSet &es) : bitset_(es.bitset_) {}
    //explicit EnumSet(const bitset_t &bt) : bitset_(bt) {}

    EnumSet &operator=(const EnumSet &es) {bitset_ = es.bitset_; return *this;}

    bool operator==(const EnumSet &es) const { return bitset_ == es.bitset_; }
    bool operator!=(const EnumSet &es) const { return bitset_ != es.bitset_; }

    void set(ET e) { bitset_.set(EnumToSizeT::get<ET>(e)); }
    EnumSet &set(ET e, bool value)
    { bitset_.set(EnumToSizeT::get<ET>(e), value); return *this; }
    EnumSet& set() { bitset_.set(); return *this; }

    void unset(ET e) { bitset_.reset(EnumToSizeT::get<ET>(e)); }
    void clear() { bitset_.reset(); }
    EnumSet& reset() { bitset_.reset(); return *this; }
    EnumSet& reset(ET e) { bitset_.reset(EnumToSizeT::get<ET>(e)); return *this; }
  
    void toggle(ET e) { bitset_.flip(EnumToSizeT::get<ET>(e)); }
    EnumSet&  flip(ET e) { bitset_.flip(EnumToSizeT::get<ET>(e)); return *this; }
    EnumSet& flip() {
        for (auto e: ET::enum_values) flip(e);
        return *this;
    }

    friend EnumSet operator~ (EnumSet es) { es.flip(); return es; }

    bool has(ET e) const { return bitset_.test(EnumToSizeT::get<ET>(e)); }
    bool hasAny(const EnumSet &es) const { return (bitset_ & es.bitset_).any(); }

    bool empty() const { return bitset_.none(); }
 
    explicit operator bool() const { return bitset_.any(); }
    size_t size() const { return bitset_.size(); }
    size_t count() const { return bitset_.count(); }
 
    EnumSet &operator|=(ET e) { set(e); return *this; }
    EnumSet &operator&=(ET e) { const bool v=bitset_.test(e); clear(); return set(e, v); }
    EnumSet &operator+=(ET e) { set(e); return *this; }
    EnumSet &operator-=(ET e) { return set(e, false); }

    EnumSet &operator|=(EnumSet es) { bitset_ |= es.bitset_; return *this; }
    EnumSet &operator&=(EnumSet es) { bitset_ &= es.bitset_; return *this; }
    EnumSet &operator+=(EnumSet es) { bitset_ |= es.bitset_; return *this; }
    EnumSet &operator-=(EnumSet es) { bitset_ &= es.bitset_.flip(); return *this; }

    friend EnumSet operator | (EnumSet es1, EnumSet es2) { es1 |= es2; return es1; }
    friend EnumSet operator & (EnumSet es1, EnumSet es2) { es1 &= es2; return es1; }
    friend EnumSet operator + (EnumSet es1, EnumSet es2) { es1 += es2; return es1; }
    friend EnumSet operator - (EnumSet es1, EnumSet es2) { es1 -= es2; return es1; }

    EnumSet operator&(ET e) { EnumSet tmp(*this); return tmp&=e; }
    EnumSet operator|(ET e) { EnumSet tmp(*this); return tmp|=e; }

    bool operator[](ET e) const { return bitset_[EnumToSizeT::get<ET>(e)]; }
   
    ///Contructor from multiple underlying enum type values
    ///@Usage: EnumSet<EnumType>(EnumValue1, EnumValue3, EnumValue5)
    template<typename... ETS>
    EnumSet(ET e, ETS... others)
    {
        bitset_.set(EnumToSizeT::get<ET>(e));
        *this |= EnumSet(others...);
    }

    uint64_t toUInt64 () const noexcept(false) { return bitset_.to_ullong(); }

    bitset_t toUnderlying() const { return bitset_; }
    static EnumSet fromUnderlying (const bitset_t& bitset) { return EnumSet(bitset); }

    /// Returns string in binary bitset format
    std::string toStringRaw() const { return bitset_.to_string(); }

    /// Returns string in a set of enum names
    std::string toString() const
    {
        std::string str = "(";
        for (auto e: ET::enum_values)
        {
            if (has(ET(e)))
            {
                if (str.length() > 1) str += ",";
                str += ET(e).toString();
            }
        }
        str += ")";
        return str;
    }

    /// \brief Get enum set from string
    static EnumSet fromString(const char* str)
    {
        EnumSet es;
        if (!str) return es;
        std::vector<std::string> substrings;
        strSplitQuoted(str, substrings);
        for (auto estr: substrings)
        {
            ET e = ET::fromString(estr.c_str());
            if (ET::isValid(e)) es.set(e);
        }
        return es;
    }

    static EnumSet fromString(const std::string & str)
    {
        return fromString(str.c_str());
    }


    friend std::ostream& operator<<(std::ostream& stream, const EnumSet& es)
    { return stream << es.toString(); }
};

template <typename ET> using EnumSet8  = EnumSet<ET, BitSet<uint8_t>>;
template <typename ET> using EnumSet16 = EnumSet<ET, BitSet<uint16_t>>;
template <typename ET> using EnumSet32 = EnumSet<ET, BitSet<uint32_t>>;
template <typename ET> using EnumSet64 = EnumSet<ET, BitSet<uint64_t>>;
#ifdef __SIZEOF_INT128__ 
template <typename ET> using EnumSet128 = EnumSet<ET, BitSet<ullong>>;
#endif

} // namespace alt
