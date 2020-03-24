#pragma once

#include <util/sysinfo/Platform.h>
#include <util/numeric/Intrinsics.h>   // for clz
#include <stddef.h>
#include <stdint.h>
#include <type_traits>
#include <string>
#include <cstring>
#include <vector>

//----------------------------------------------------------------------------
// string utils
//----------------------------------------------------------------------------
namespace alt
{
template <int N> ALT_INLINE void strCpy (char* d, const char* s) 
{ std::strncpy(d,s, N); }
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
// aligned by 16. Oterwise, called the unaligned version
const char* fastStrChrAligned(const char* s, char ch);
size_t fastStrLenAligned(const char* s);
uint64_t fastSumAligned(const uint8_t* bytes, int sz);

const char* fastStrChr(const char* s, char ch);
size_t fastStrLen(const char* s);
uint64_t fastSum(const uint8_t* bytes, int sz);

size_t strHash(const char * str);
size_t strHash(const char * str, size_t length);
size_t strHash (const char *str, size_t length, size_t seed);

/// \brief Split a string into sub-strings with the given separator
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
/// \return the number of characters scanned
size_t strSplit (
    const char * str,
    std::vector<std::string>& substrings,
    size_t length = 0,
    char separator = ',',
    char terminator = '\0',
    bool skip_leading_sp = true,
    bool skip_trailing_sp = true);

size_t strSplitQuoted (
    const char * str,
    std::vector<std::string>& substrings,
    char separator = ',',
    bool skip_leading_sp = true,
    bool skip_trailing_sp = true);

ALT_INLINE bool isDigit (char ch)
{
    return (bool)(ch>='0' && ch<='9');
};

ALT_INLINE bool isOctDigit (char ch)
{
    return (bool)(ch>='0' && ch<='7');
};

ALT_INLINE bool isBinDigit (char ch)
{
    return (bool)(ch=='0' || ch=='1');
};

ALT_INLINE bool isHexDigit (char ch)
{
    return (bool)
        ((ch>='0' && ch<='9') || (ch>='A' && ch<='F') || (ch>='a' && ch<='f'));
};

ALT_INLINE size_t uCharBytes(char ch)
{
    return clz(~(uint32_t(uint8_t(ch))<<24));
}

constexpr char INVALID_HEX_DIGIT = 0X10;

char hexDigit (char ch);

size_t scanUTF8String(const char * utf8Char, char32_t& wch);

} // namespace alt