
#include "Intrinsics.h"
#include <iostream>
#include <cstring>
#include <cassert>

#if defined (__SSE2__)
// use -msse2 for g++
#include <emmintrin.h>
#include <immintrin.h>
#endif

namespace alt
{
#if not defined (__GNUC__)
int ffs (uint32_t x)
{
    // De Bruijn Multiply table
    static const int DeBruijnTable32[32] = 
    {
       1, 2, 29, 3, 30, 15, 25, 4, 31, 23, 21, 16, 26, 18, 5, 9, 
       32, 28, 14, 24, 22, 20, 17, 8, 27, 13, 19, 7, 12, 6, 11, 10
    };
    return DeBruijnTable32[((unsigned)((x & -x) * 0x077CB531U)) >> 27];
}

int ffs (uint64_t x)
{
    static const int DeBruijnTable64[64] = 
    {
        0, 47, 1, 56, 48, 27, 2, 60, 57, 49, 41, 37, 28, 16, 3, 61,
        54, 58, 35, 52, 50, 42, 21, 44, 38, 32, 29, 23, 17, 11, 4, 62,
        46, 55, 26, 59, 40, 36, 15, 53, 34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30, 9, 24, 13, 18, 8, 12, 7, 6, 5, 63
    };
    return DeBruijnTable64[((uint32_t)((v ^ (v-1)) * 0x03F79D71B4CB0A89U)) >> 58];
}
template <typename T> int clz_T (T x)
{
    int n = 0;
    if (x == 0) return sizeof(x) * 8;
    T mask = T(1) << (sizeof(x) * 8 - 1);
    for (; (x & mask)==T(0); ++n, x<<1);
    return n;
}
int clz_T(uint32_t n) { return clz_T(n); }
int clz_T(uint64_t n) { return clz_T(n); }

template <typename T> int bitsCount_T(T n) 
{ 
    int count = 0; 
    while (n) { count += n & 1; n >>= 1; } 
    return count; 
}

int bitsCount(uint32_t n) { return bitsCount_T(n); }
int bitsCount(uint64_t n) { return bitsCount_T(n); }
#endif

} // namespace alt
