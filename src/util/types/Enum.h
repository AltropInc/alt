#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file Enum.h
 * @library alt_util
 * @brief Defines reflective Enum Type that supports:
 *    - obtaining enum value from string (fromString)
 *    - obtaining enum value sequence (enum_values) for iteration
 *    - getting number of enum values (count)
 *    - getting maximum enum value (max)
 *    - getting invalid enum value (invalid)
 *    - checking if the value is valid (isValid)
 *    - obtaining enum value from underlying type (fromUnderlying)
 *    - converting to underlying type (toUnderlying)
 *    - converting to string (toString)
 *    - ostream operators for enums
 */

#include "ValueWrapper.h"

#include <stdint.h>
#include <stddef.h>
//#include <unordered_map>
//#include <iostream>

namespace alt
{
/**
 * \class EnumBase
 * \brief Base of Enum to sort enum name for efficient search. These functions are lazy
 * invoked
 */
class EnumBase
{
  protected:
    static int fromString(const char** name_list, const int* name_indice, const char* enum_name, size_t enum_number);
    static void setEnumNames(const char *names_buf, const char**names, size_t enum_num);
};


template<typename EnumT> class EnumType {};

#define NUMARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

/**
 * \brief Macro to define reflective Enum types.
 * \details Usage:
 * ENUM(Digit, uint8_t, Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine);
*/
#define ENUM(NAME, UnderlingType, ...) \
class NAME; \
template<> class EnumType<NAME>: \
    public alt::OPIncrementable<UnderlingType,NAME>, public alt::EnumBase \
{ public: \
    enum enum_type: UnderlingType { __VA_ARGS__ }; \
    constexpr EnumType(UnderlingType val): OPIncrementable(val) {} \
}; \
class NAME: public EnumType<NAME> \
{ public: \
  using underlying_type = UnderlingType; \
    constexpr static underlying_type _enum_number = NUMARGS(__VA_ARGS__); \
    inline static const char* _type_name = #NAME; \
    inline static const char* _names[_enum_number] { nullptr }; \
    inline static int _name_indice[_enum_number] { -1 }; \
    inline static enum_type enum_values [] {__VA_ARGS__}; \
    constexpr bool operator == (enum_type oth) const { return value_ == oth; }; \
    constexpr bool operator != (enum_type oth) const { return value_ == oth; }; \
    constexpr static size_t count() { return _enum_number; } \
    constexpr static NAME max() { return NAME(_enum_number-1); } \
    constexpr static NAME invalid() { return NAME(_enum_number); } \
    constexpr static bool isValid(NAME ev) { return ev.value_>=0 && ev.value_<_enum_number; } \
    constexpr static underlying_type toUnderlying(NAME ev) { return ev.value_; } \
    constexpr static NAME fromUnderlying(underlying_type v) { return NAME(v); } \
    constexpr NAME(): EnumType(0){} \
    constexpr NAME(const NAME & v): EnumType(v.value_){} \
    constexpr NAME(underlying_type v): EnumType(v){} \
    constexpr NAME(enum_type ev): EnumType(UnderlingType(ev)){} \
    const char* toString() const { init(); return _names[value_]; } \
    static NAME fromString(const char* name) { init(); int res=EnumBase::fromString(_names, _name_indice, name, _enum_number); return res<0?invalid():fromUnderlying(res); } \
    template <typename StrOT> friend StrOT& operator << (StrOT& sot, NAME ev) { sot << ev.toString(); return sot; } \
  private: \
    inline static char _names_buf[] { #__VA_ARGS__ }; \
    static void init() { if (!_names[0]) EnumBase::setEnumNames(_names_buf, _names, _enum_number); } \
}

} // namespace alt
