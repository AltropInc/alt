#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file StrUtils.h
 * @library alt_util
 * @brief string utility function
*/

#include "StrBuffer.h"

#include <util/system/Platform.h>
#include <util/numeric/Intrinsics.h>   // for clz
#include <stddef.h>
#include <stdint.h>
#include <type_traits>
#include <string>
#include <cstring>
#include <vector>
#include <ctype.h>

//----------------------------------------------------------------------------
// string utils
//----------------------------------------------------------------------------
namespace alt
{
template <int N> ALT_INLINE void strCpy (char* d, const char* s) 
{ std::strncpy(d, s, N); }
template <> ALT_INLINE void strCpy<1> (char* d, const char* s) 
{ *d=*s; }
template <> ALT_INLINE void strCpy<2> (char* d, const char* s) 
{ *((int16_t*)d)=*((int16_t*)s); }
template <> ALT_INLINE void strCpy<3> (char* d, const char* s)
{ strCpy<2>(d+4,s+4); *(d+4)=*(s+4); }
template <> ALT_INLINE void strCpy<4> (char* d, const char* s)
{ *((int32_t*)d)=*((int32_t*)s); }
template <> ALT_INLINE void strCpy<5> (char* d, const char* s)
{ strCpy<4>(d,s); *(d+4)=*(s+4); }
template <> ALT_INLINE void strCpy<6> (char* d, const char* s)
{ strCpy<4>(d,s); strCpy<2>(d+4,s+4); }
template <> ALT_INLINE void strCpy<7> (char* d, const char* s)
{ strCpy<4>(d,s); strCpy<3>(d+4,s+4); }
template <> ALT_INLINE void strCpy<8> (char* d, const char* s)
{ *((int64_t*)d)=*((int64_t*)s); }
template <> ALT_INLINE void strCpy<9> (char* d, const char* s)
{ strCpy<8>(d,s); *(d+8)=*(s+8); }
template <> ALT_INLINE void strCpy<10> (char* d, const char* s)
{ strCpy<8>(d,s); strCpy<2>(d+8,s+8); }
template <> ALT_INLINE void strCpy<11> (char* d, const char* s)
{ strCpy<8>(d,s); strCpy<3>(d+8,s+8); }
template <> ALT_INLINE void strCpy<12> (char* d, const char* s)
{ strCpy<8>(d,s); strCpy<4>(d+8,s+8); }
template <> ALT_INLINE void strCpy<13> (char* d, const char* s)
{ strCpy<8>(d,s); strCpy<5>(d+8,s+8); }
template <> ALT_INLINE void strCpy<14> (char* d, const char* s)
{ strCpy<8>(d,s); strCpy<6>(d+8,s+8); }
template <> ALT_INLINE void strCpy<15> (char* d, const char* s)
{ strCpy<8>(d,s); strCpy<7>(d+8,s+8); }
template <> ALT_INLINE void strCpy<16> (char* d, const char* s)
{ strCpy<8>(d,s); strCpy<8>(d+8,s+8); }

ALT_INLINE bool str2Equal (const char* x, const char* y)
{ return *((int16_t*)x)==*((int16_t*)y); }
ALT_INLINE bool str3Equal (const char* x, const char* y)
{ return str2Equal(x,y) && *(x+2)==*(y+2); }
ALT_INLINE bool str4Equal (const char* x, const char* y)
{ return *((int32_t*)x)==*((int32_t*)y); }
ALT_INLINE bool str5Equal (const char* x, const char* y)
{ return str4Equal(x,y) && *(x+2)==*(y+2); }
ALT_INLINE bool str6Equal (const char* x, const char* y)
{ return str4Equal(x,y) && str2Equal(x+4,y+4); }
ALT_INLINE bool str7Equal (const char* x, const char* y)
{ return str4Equal(x,y) && str3Equal(x+4,y+4); }
ALT_INLINE bool str8Equal (const char* x, const char* y)
{ return *((int64_t*)x)==*((int64_t*)y); }
ALT_INLINE bool str9Equal (const char* x, const char* y)
{ return str8Equal(x,y) && *(x+8)==*(y+8); }
ALT_INLINE bool str10Equal (const char* x, const char* y)
{ return str8Equal(x,y) && str2Equal(x+8,y+8); }

template<int N> ALT_INLINE bool strEqual (const char* x, const char* y)
{ return std::strncmp(x,y,N)==0; }
template<> ALT_INLINE bool strEqual<1> (const char* x, const char* y)
{ return *x==*y; }
template<> ALT_INLINE bool strEqual<2> (const char* x, const char* y)
{ return *((int16_t*)x)==*((int16_t*)y); }
template<> ALT_INLINE bool strEqual<3> (const char* x, const char* y)
{ return str2Equal(x,y) && *(x+2)==*(y+2); }
template<> ALT_INLINE bool strEqual<4> (const char* x, const char* y)
{ return *((int32_t*)x)==*((int32_t*)y); }
template<> ALT_INLINE bool strEqual<5> (const char* x, const char* y)
{ return str4Equal(x,y) && *(x+2)==*(y+2); }
template<> ALT_INLINE bool strEqual<6> (const char* x, const char* y)
{ return str4Equal(x,y) && str2Equal(x+4,y+4); }
template<> ALT_INLINE bool strEqual<7> (const char* x, const char* y)
{ return str4Equal(x,y) && str3Equal(x+4,y+4); }
template<> ALT_INLINE bool strEqual<8> (const char* x, const char* y)
{ return *((int64_t*)x)==*((int64_t*)y); }
template<> ALT_INLINE bool strEqual<9> (const char* x, const char* y)
{ return str8Equal(x,y) && *(x+8)==*(y+8); }
template<> ALT_INLINE bool strEqual<10> (const char* x, const char* y)
{ return str8Equal(x,y) && str2Equal(x+8,y+8); }
template<> ALT_INLINE bool strEqual<11> (const char* x, const char* y)
{ return str8Equal(x,y) && str3Equal(x+8,y+8); }
template<> ALT_INLINE bool strEqual<12> (const char* x, const char* y)
{ return str8Equal(x,y) && str4Equal(x+8,y+8); }
template<> ALT_INLINE bool strEqual<13> (const char* x, const char* y)
{ return str8Equal(x,y) && str5Equal(x+8,y+8); }
template<> ALT_INLINE bool strEqual<14> (const char* x, const char* y)
{ return str8Equal(x,y) && str6Equal(x+8,y+8); }
template<> ALT_INLINE bool strEqual<15> (const char* x, const char* y)
{ return str8Equal(x,y) && str7Equal(x+8,y+8); }
template<> ALT_INLINE bool strEqual<16> (const char* x, const char* y)
{ return str8Equal(x,y) && str8Equal(x+8,y+8); }

// Call these functions if you know the start buffer is already
// aligned by 16. Otherwise, called the unaligned version
const char* fastStrChrAligned(const char* s, char ch);
size_t fastStrLenAligned(const char* s);
uint64_t fastSumAligned(const uint8_t* bytes, int sz);

// unaligned versions
const char* fastStrChr(const char* s, char ch);
size_t fastStrLen(const char* s);
uint64_t fastSum(const uint8_t* bytes, int sz);

/// \brief conver std::array<char,8> into 64-bit integer for template parameter
/// usage to overcome the limit of template parameter not being a string
constexpr static uint64_t strToNameId(std::array<char,8> name)
{
    uint64_t res = 0;
    for (auto ch: name)
    {
        if (!ch) break;
        res = (res<<8) + uint8_t(ch);
    }
    return res;
}

/// \brief conver  64-bit integer into std::array<char,9> for template parameter
/// usage to overcome the limit of template parameter not being a string
constexpr std::array<char,9> strFromNameId(uint64_t id)
{
    std::array<char,9> res {0};
    int i = 0;
    int j = 0;
    for (; i<8; ++i)
    {
        char ch = char ((id & 0xFF00000000000000UL) >> 56);
        if (ch) res[j++] = ch;
        id <<= 8;
    }
    if (j<8) res[j] = 0;
    return res;
}

/// \brief Split a string into substrings with the given separator
/// \param str The string to be slipt
/// \param substrings The containder for the output of the split substrings
/// \param length The length of the string to be split. When 0, the length is
/// determined by scanning the null terminator of the string to be split
/// \param separator The separater char to divide substrings
/// \param terminator The terminator fo the string to be split
/// \param skip_leading_sp Flag to indicating whether the leading spaces of the
/// substring should be skipped
/// \param skip_trailing_sp Flag to indicating whether the trailing spaces of the
/// substring should be skipped
/// \return the number of characters scanned in str
size_t strSplit (
    const char * str,
    std::vector<std::string>& substrings,
    size_t length = 0,
    char separator = ',',
    char terminator = '\0',
    bool skip_leading_sp = true,
    bool skip_trailing_sp = true);

/// \brief split a quoted string into a list of substrings
/// \param str the buffer contains quotes strings. The quote can be (), {}, [], or "".
/// for example "{string1, string2, string2}", or "\"string1, string2, string2\""
/// \param substrings The containder for the output of the split substrings
/// \param separator the separator in the string buffer between substrings
/// \param skip_leading_sp when true, skip leading spaces in a substring
/// \param skip_trailing_sp when true, skip trailing spaces in a substring
/// \return the number of characters scanned in str
size_t strSplitQuoted (
    const char * str,
    std::vector<std::string>& substrings,
    char separator = ',',
    bool skip_leading_sp = true,
    bool skip_trailing_sp = true);

/// \brief check if a character is for a decimal digit
/// \param ch the character to check
template <typename CHAR_T>
ALT_INLINE bool isDigit (CHAR_T ch)
{
    return ch>='0' && ch<='9';
};

/// \brief check if a character is for an oct digit
/// \param ch the character to check
template <typename CHAR_T>
ALT_INLINE bool isOctDigit (CHAR_T ch)
{
    return ch>='0' && ch<='7';
};

/// \brief check if a character is for a binary digit
/// \param ch the character to check
template <typename CHAR_T>
ALT_INLINE bool isBinDigit (CHAR_T ch)
{
    return ch=='0' || ch=='1';
};

/// \brief check if a character is for a hex digit
/// \param ch the character to check
template <typename CHAR_T>
ALT_INLINE bool isHexDigit (CHAR_T ch)
{
    return (ch>='0' && ch<='9') || (ch>='A' && ch<='F') || (ch>='a' && ch<='f');
};

/// \brief Get number of bytes of an UTF-8 encoded string by looking its first byte
/// \param ch the first character byte of an UTF-8 encoded string
ALT_INLINE size_t uCharBytes(char ch)
{
    return clz(~(uint32_t(uint8_t(ch))<<24));
}

constexpr char INVALID_HEX_DIGIT = 0X10;
/// \brief convert a hex character (0~9, A~F, a~f) to integer (0~15)
/// \param ch the hex character
/// \return integer 0~15, or INVALID_HEX_DIGIT if oaram ch is not a hex character
uint8_t hexDigit (char ch);

/// \brief get the first character from an UTF8 string
/// \param utf the UTF8 string
/// \param wch the output of the scanned character, in 4-byte unicode
/// \return the number of scanned bytes of the character
size_t scanUTF8String(const char * utf8Char, alt_char_t& wch);

/// \brief convert 4-byte unicode into an UTF8 string
/// \param wch the 4-byte unicode
/// \param utf8Char_buffer the UTF8 string buffer
/// \param buffer_length the size of the TF8 string buffer
/// \return number of converted bytes in utf8Char_buffer
size_t wcharToUTF8(alt_char_t wch, char * utf8Char_buffer, size_t buffer_length);

/// \brief convert 4-byte unicode string into an UTF8 string
/// \param wch_str the 4-byte unicode string
/// \param length number of unicode chars in the string
/// \param utf8Char_buffer the UTF8 string buffer
/// \param buffer_length the size of the TF8 string buffer
/// \return number of converted bytes in utf8Char_buffer
size_t wcharStrToUTF8(const alt_char_t *wch_str, size_t length, char * utf8Char_buffer, size_t buffer_length);

/// \brief convert UFt8 string to  4-byte unicode string
/// \param utf8_str points to a UTF8 encoded string, null terminated
/// \return the u32 string in unicode
std::u32string toU32String(const char * utf8_str);

template <typename StrT>
std::u32string toU32String(const StrT & utf8_str)
{
    return toU32String(utf8_str.c_str());
}

/// \brief tells if a UTF8 encoded string is a basic string
bool isBasicString(const char * utf8_str);

template <typename StrT>
bool isBasicString(const StrT & utf8_str)
{
    return isBasicString(utf8_str.c_str());
}

/// \brief compare two UTF8 strings.
/// \param str1 the first string to compare.
/// \param str2 the second string to compare.
/// \param case_sensitive when true, do the caseless comparison.
/// \param number_as_whole when true and a number starts at the same position of
/// two strings, the compare will use the number as whole instead of comparing
/// digits individually. For instance, "item(12)" is greater than "item(3)"
/// \note this does not support the case of comparsion involving multiple
/// characters vs. single character such as between "ss" and “ß” in German.
/// In these cases they are considered not equal.
int strCmp (const char* str1, const char* str2, bool case_sensitive, bool number_as_whole);

/// \brief Forward trim a c string pointer (skip ASCII white spaces only)
/// \param cp pointer to a c string to trim. This cannot be nullptr
/// \return pointer to the first non-whilespace character or the null therminator
inline const char* strTrimForward(const char* cp) noexcept (false)
{
    while (isspace(*cp)) ++cp;
    return cp;
}

/// \brief Backward trim a c string pointer (skip ASCII white spaces only)
/// \param cp pointer to a c string to trim. This cannot be nullptr
/// \param length length of the c string
/// \return pointer to the position after the last first non whilespace character
//  or the begining of the string
const char* strTrimBackward(const char* cp, size_t length) noexcept (false);

/// \brief Backward trim a c string pointer (skip ASCII white spaces only)
/// \param cp pointer to a c string to trim. This cannot be nullptr
/// \return pointer to the position after the last first non whilespace character
//  or the begining of the string
inline const char* strTrimBackward(const char* cp) noexcept (false)
{
    return strTrimBackward(cp, strlen(cp));
}

/// \brief Forward and Backward trim a c string pointer (skip ASCII white spaces only)
/// \param str pointer to a c string to trim
/// \param length length of the c string
/// \param begin output param points to the beginning of the trimmed string
/// \param end output param points to the end of the trimmed string
inline void strTrim(const char* str, size_t length, const char*& begin, const char*& end)
{
    if (str==nullptr || length==0)
    {
        begin = end = nullptr;
        return;
    }
    begin = str;
    end = str + length - 1;
    while (begin < end && isspace(*begin)) ++begin;
    while (begin < end && isspace(*end)) --end;
    if (begin==end && isspace(*begin))
    {
        begin = end = nullptr;
    }
}

/// \brief Forward and Backward trim a string (skip ASCII white spaces only)
/// \param str the input string for tirim
/// \return the str being trimmed
template <typename StrT>
inline StrT strTrim(const StrT& str)
{
    const char* begin;
    const char* end;
    strTrim(str.c_str(), str.length(), begin, end);
    return StrT(begin, size_t(end-begin)+1);
}

/// \brief Trim copy (remove leading and trailing spaces) for any string type
/// that has methods c_str(), length(), and constructor(char*, size_t)
/// \param str the source string to be trimmed
/// \return string being trimmed
template <typename StrT>
inline StrT strTrimCpy(StrT const & str)
{
    const char *startp, *endp;
    strTrim(str.c_str(), str.length(), startp, endp);
    if (startp && endp)
    {
        return StrT(startp, size_t(endp-startp+1));
    }
    return StrT();
}

/// \brief Trim copy (remove leading and trailing spaces) on c strings
/// \param dest pointer to dest buffer for the trimmed string
/// \param source pointer to source string to be trimmed
inline void strTrimCpy(char* dest, const char* source)
{
    const char *startp, *endp;
    strTrim(source, strlen(source), startp, endp);
    if (startp && endp)
    {
        strncmp(dest, startp, size_t(endp-startp+1));
    }
    else
    {
        *dest = '\0';
    }
}

/// \brief coverts unicode char into a formatted string with escape sequence
/// \param wch the char value to be converted
/// \param StrBuf the buffer to hold the convert result
void uCharToFormattedString(alt_char_t wch, StrBuf& buffer);
inline std::string uCharToFormattedString(alt_char_t wch)
{
    char buffer[16];
    StrBuf str_buf(buffer, 16);
    uCharToFormattedString(wch, str_buf);
    return std::string(buffer, str_buf.length());
}

/// \brief coverts uft8 string into a formatted string with escape sequence
/// \param u8_str the utf-8 string to be converted
/// \param res the buffer to hold the converted result
/// \param use_unicode_seq when true, utf-8 encoded sequence will be converted into
/// unicode escape sequence if the character value is greater than 0x100. Otherwise,
/// the output will keep the utf-8 encoded sequence
void u8StrToFormattedString(const char* u8_str, std::string& res, bool use_unicode_seq);

} // namespace alt