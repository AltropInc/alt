#pragma once

#include <util/sysinfo/Platform.h>
#include <stddef.h>
#include <stdint.h>
#include <type_traits>
#include <limits>
//----------------------------------------------------------------------------
// Define byte specific integers
//----------------------------------------------------------------------------
namespace alt
{
//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
#ifndef INT64_MAX
constexpr int64_t  INT64_MAX  = std::numeric_limits<int64_t>::max();  // 9223372036854775807ULL
constexpr int64_t  INT64_MIN  = std::numeric_limits<int64_t>::min();  // -9223372036854775807ULL
constexpr uint64_t UINT64_MAX = std::numeric_limits<uint64_t>::max(); // 18446744073709551615ULL
constexpr int32_t  INT32_MAX  = std::numeric_limits<int32_t>::max();  // 2147483647
constexpr int32_t  INT32_MIN  = std::numeric_limits<int32_t>::min();  // -2147483647
constexpr uint32_t UINT32_MAX = std::numeric_limits<uint32_t>::max(); // 4294967295
constexpr int16_t  INT16_MAX  = std::numeric_limits<int16_t>::max();  // 32767
constexpr int16_t  INT16_MIN  = std::numeric_limits<int16_t>::min();  // -32767
constexpr uint16_t UINT16_MAX = std::numeric_limits<uint16_t>::max(); // 65535
constexpr int8_t   INT8_MAX   = std::numeric_limits<int8_t>::max();   //  127
constexpr int8_t   INT8_MIN   = std::numeric_limits<int8_t>::min();   // -127
constexpr uint8_t  UINT8_MAX  = std::numeric_limits<uint8_t>::max();  // 255
#endif
constexpr double   DOUBLE_MIN  = std::numeric_limits<double>::min();
constexpr double   DOUBLE_MAX  = std::numeric_limits<double>::max();
constexpr float    FLOAT_MIN  = std::numeric_limits<float>::min();
constexpr float    FLOAT_MAX  = std::numeric_limits<float>::max();

constexpr int64_t  INT64_NaN  = std::numeric_limits<int64_t>::min();
constexpr int32_t  INT32_NaN  = std::numeric_limits<int32_t>::min();
constexpr int16_t  INT16_NaN  = std::numeric_limits<int16_t>::min();
constexpr int8_t   INT8_NaN   = std::numeric_limits<int8_t>::min();
constexpr int      INT_NaN    = std::numeric_limits<int>::min();
constexpr double   DOUBLE_NaN = std::numeric_limits<double>::quiet_NaN();
constexpr float    FLOAT_NaN  = std::numeric_limits<float>::quiet_NaN();

//----------------------------------------------------------------------------
// constLog2, constPower2 log2Floor, log2Ceil, and power2Next
//----------------------------------------------------------------------------
template <typename T>
constexpr T constLog2(T n) { return (n>1) ? 1 + constLog2(n>>1) : 0; }
template <typename T>
constexpr T constPower2(T n) { return (n>0) ? constPower2(n-1)<<1 : 1; }

#if defined (__GNUC__)
ALT_INLINE int log2Floor (uint64_t n) {return 63 - __builtin_clzl(uint64_t(n));}
ALT_INLINE int log2Floor (uint32_t n) {return 31 - __builtin_clz(uint32_t(n));}

template <typename T>
ALT_INLINE T power2Next (T n)
{
    return n-2 <= 0 ? T(n) : (T(1) << log2Floor(n));
}

#else

template <typename T>
ALT_INLINE int log2Floor (T x)
{
  T i;
  int position =0;
  for ( i = (x >> 1); i != 0; ++position)
     i >>= 1;
  return position;
}

template <typename T>
ALT_INLINE T power2Next (int n)
{
	T y = 1;
	if (n==0) return 0;
	// Keep doubling y until we are done
	while (y <= n) y += y;
	return y;
}
#endif

template <typename T>
ALT_INLINE int log2Ceil (T n)
{
    return n > 1 ? log2Floor(n-1)+1 : 0;
}

//----------------------------------------------------------------------------
// Expr table
//----------------------------------------------------------------------------
constexpr uint64_t s_exp10[] = {
 	1LL,
    10LL,
	100LL,
	1000LL,
	10000LL,
	100000LL,
	1000000LL,
	10000000LL,
	100000000LL,
	1000000000LL,
	10000000000LL,
	100000000000LL,
	1000000000000LL,
	10000000000000LL,
	100000000000000LL,
	1000000000000000LL,
	10000000000000000LL,
	100000000000000000LL,
	1000000000000000000LL
};
constexpr size_t s_exp10_length = sizeof(s_exp10)/sizeof(uint64_t);

//----------------------------------------------------------------------------
// Double digits table for fast string to numeric conversion
//----------------------------------------------------------------------------
constexpr char s_double_digits[] = {
	"00010203040506070809"
	"10111213141516171819"
	"20212223242526272829"
	"30313233343536373839"
	"40414243444546474849"
	"50515253545556575859"
	"60616263646566676869"
	"70717273747576777879"
	"80818283848586878889"
	"90919293949596979899"
};

//----------------------------------------------------------------------------
// constAlign align must be the value of 2, 4, 8, 16, 32, 64, ...
//----------------------------------------------------------------------------
template <typename T>
constexpr T constAlign(T n, size_t align) { return  n + (-n & (align-1));}

template <typename T>
constexpr const T* constAlign(const T* p, size_t align)
{ return (const T*) constAlign(intptr_t(p), align); }

template <typename T>
constexpr T* constAlign(T* p, size_t align)
 { return  (T*) constAlign(intptr_t(p), align); }

//----------------------------------------------------------------------------
// Bitsets
//----------------------------------------------------------------------------
#if defined (__GNUC__)
///Returns the number of leading 0-bits in x, starting at the most significant
///bit position. If x is 0, the result is undefined.
ALT_INLINE int clz (uint32_t n) { return __builtin_clz(n); }
ALT_INLINE int clz (uint64_t n) { return __builtin_clzl(n); }

///Returns one plus the index of the least significant 1-bit of x,
///or if x is zero, returns zero.
ALT_INLINE int ffs (uint32_t n) { return __builtin_ffs(n); }
ALT_INLINE int ffs (uint64_t n) { return __builtin_ffsl(n); }

///Returns the number of trailing 0-bits in x, starting at the least significant
///bit position. If x is 0, the result is undefined.
ALT_INLINE int ctz (uint32_t n) { return __builtin_ctz(n); }
ALT_INLINE int ctz (uint64_t n) { return __builtin_ctzl(n); }

///Returns the number of 1-bits in x
ALT_INLINE int bitsCount (uint32_t x) { return __builtin_popcount(x); }
ALT_INLINE int bitsCount (uint64_t x) { return __builtin_popcountl(x); }
#else
int clz (uint32_t x);
int ffs (uint32_t x);
int ctz (uint32_t x) return ffs(x) -1; }
ALT_INLINE int bitsCount (int32_t x)
ALT_INLINE int bitsCount (int64_t x)
#endif

template <typename UIntT> constexpr int constCLZ (UIntT x)
{
    auto constCLZ_N0 = [&x] (UIntT x, int r) ->int {
        return (x & (1 << (sizeof(UIntT)*8-1))) ? r : constCLZ(x << 1, r+1);
    };
    return x==0 ? sizeof(UIntT)*8 : constCLZ_N0(x, 0);
}
template <typename UIntT> constexpr int constFFS (UIntT x)
{
    return x==0 ? 0 : ((x & 1) ? 1 : constFFS(x >> 1) + 1);
}
template <typename UIntT> constexpr int constCTZ (UIntT x) { return constFFS(x)-1; }

template<typename T>
ALT_INLINE constexpr T clearBits (T val, T upd_bits) { return val & ~upd_bits; }
template<typename T>
ALT_INLINE constexpr T setBits (T val, T upd_bits) { return val | upd_bits; }
template<typename T>
ALT_INLINE constexpr T toggleBits (T val, T upd_bits) { return val ^ upd_bits; }
template<typename T>
ALT_INLINE constexpr T updateBits (T val, T upd_bits, bool set)
{ return (val & ~upd_bits) | (val & -T(set)); }

template<typename T>
ALT_INLINE constexpr T isel(bool cond, const T& v1, const T& v2)
{ return v1 ^ ((v1 ^ v2) & -T(cond)); }

//----------------------------------------------------------------------------
// ConstSwapBytes
//----------------------------------------------------------------------------
ALT_INLINE constexpr uint16_t ConstSwapBytes (uint16_t x)
{
   return ((x & 0XFF00) >> 8) | ((x & 0X00FF) << 8);
}

ALT_INLINE constexpr uint32_t ConstSwapBytes (uint32_t x)
{
   return uint32_t(ConstSwapBytes(uint16_t((x & 0XFFFF0000) >> 16))) |
          uint32_t((ConstSwapBytes(uint16_t(x & 0X0000FFFF))) << 16);
}

ALT_INLINE constexpr uint64_t ConstSwapBytes (uint64_t x)
{
   return uint64_t(ConstSwapBytes(uint32_t((x & 0XFFFFFFFF00000000) >> 32))) |
          (uint64_t(ConstSwapBytes(uint32_t(x & 0X00000000FFFFFFFF))) << 32);
}

//----------------------------------------------------------------------------
// Floats and Doubles
//----------------------------------------------------------------------------
template <typename T>
class FloatChore
{
    using INT = typename std::conditional<
                std::is_same<T, float>::value, uint32_t, uint64_t>::type;
    constexpr static INT SIGNBIT = 1<<(sizeof(INT)-1);
    constexpr static INT INTMASK = ~SIGNBIT;
    constexpr ALT_INLINE static INT castToInt(T val)
    { return reinterpret_cast<INT&>(val); }
    constexpr ALT_INLINE static T castFromInt(INT val)
    { return reinterpret_cast<T&>(val); }
  public:
    constexpr ALT_INLINE static T abs (T val)
    { return castFromInt(castToInt(val)&INTMASK); }
    constexpr ALT_INLINE static bool is0 (T val, T e)
    { return castToInt(val)&INTMASK < castToInt(e); }
    constexpr ALT_INLINE static T fsel(bool c, T v1, T v2)
    { return castFromInt(isel(c, castToInt(v1),castToInt(v2))); }
};

using fchore = FloatChore<float>;
using dchore = FloatChore<double>;

//----------------------------------------------------------------------------
// swap_bytes
//----------------------------------------------------------------------------

#if defined(__GLIBC__)
#include <byteswap.h>
ALT_INLINE T swapBytes (uint_16 v=x) return { __bswap_16(x); }
ALT_INLINE T swapBytes (uint_32 v=x) return { __bswap_32(x); }
ALT_INLINE T swapBytes (uint_64 v=x) return { __bswap_64(x); }
#else
template <typename T>
ALT_INLINE T swapBytes (T x) { return ConstSwapBytes(x); }
#endif

//----------------------------------------------------------------------------
// Edianness conversion utils
//----------------------------------------------------------------------------
#if (ALT_ENDIAN==ALT_ENDIAN_LITTLE)
template <typename T> ALT_INLINE T htobe (T n) { return swapBytes(n); }
template <typename T> ALT_INLINE T htole (T n) { return n; }
template <typename T> ALT_INLINE T betoh (T n) { return swapBytes(n); }
template <typename T> ALT_INLINE T letoh (T n) { return n; }
#else
template <typename T> ALT_INLINE T htobe (T n) { return n; }
template <typename T> ALT_INLINE T htole (T n) { return swapBytes(n); }
template <typename T> ALT_INLINE T betoh (T n) { return n; }
template <typename T> ALT_INLINE T letoh (T n) { return swapBytes(n); }
#endif

//----------------------------------------------------------------------------
// Int Hash
//----------------------------------------------------------------------------
// Robert Jenkins' 32 bit integer hash function
inline uint32_t RJIntHash(uint32_t a)
{
    a = (a+0x7ed55d16) + (a << 12);
    a = (a^0xc761c23c) ^ (a >> 19);
    a = (a+0x165667b1) + (a << 5);
    a = (a+0xd3a2646c) ^ (a << 9);
    a = (a+0xfd7046c5) + (a << 3);
    a = (a^0xb55a4f09) ^ (a >> 16);
    return a;
}

// Thomas Wang's 32 bit integer hash function
inline uint32_t TWIntHash(uint32_t a)
{
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

} // namespace alt