
#include "StrUtils.h"
#include <iostream>
#include <cstring>
#include <cassert>
#include <vector>

#if defined (__SSE2__)
// use -msse2 for g++
#include <emmintrin.h>
#include <immintrin.h>
#endif

namespace alt
{
const char* fastStrChrAligned(const char* s, char ch)
{
#if defined (__SSE2__)
    __m128i zero = _mm_setzero_si128();
    __m128i cx16 = _mm_set1_epi8(ch); // (ch) replicated 16 times.
    while (1) 
    {
        __m128i x = _mm_loadu_si128((__m128i const *)s);
        __m128i u = _mm_cmpeq_epi8(cx16, x);
        __m128i v = _mm_cmpeq_epi8(zero, x);
        int m1 = _mm_movemask_epi8(u);
        int m0 = _mm_movemask_epi8(v);
        if (m1 && (m1 < m0 || m0==0))
        {
            return s + ctz(uint32_t(m1));
            break;
        }
        if (m0)
        {
            return nullptr;
        }
        s += 16;
    }
#else
    return strchr(s,ch);
#endif
}

const char* fastStrChr(const char* s, char ch)
{
#if defined (__SSE2__)
    const char* aligned_addr = constAlign(s, 16);
    while (s < aligned_addr)
    {
        if (*s==0)
        {
            return nullptr;
        }
        if (*s==ch)
        {
            return s;
        }
        ++s;   
    }
    return fastStrChrAligned(s, ch);
#else
    return strchr(s,ch);
#endif
}

size_t fastStrLenAligned(const char* s)
{
#if defined (__SSE2__)
    size_t len(0);
    __m128i zero = _mm_setzero_si128();
    __m128i cx16 = _mm_set1_epi8(0);
    while (1) 
    {
        __m128i x = _mm_load_si128((__m128i const *)s);
        x = _mm_cmpeq_epi8(x, zero);
        int m = _mm_movemask_epi8(x);
        if (m)
        {
            len += ctz(uint32_t(m));
            break;
        }
        s += 16;
        len += 16;
    }
    return len;
#else
    return strlen(s);
#endif
}

size_t fastStrLen(const char* s)
{
#if defined (__SSE2__)
    size_t len(0);
    const char* aligned_addr = constAlign(s, 16);
    while (s < aligned_addr)
    {
        if (*s++==0)
        {
            return len;
        }
        ++len;   
    }
    return len + fastStrLenAligned(s);
#else
    return strlen(s);
#endif
}

uint64_t fastSumAliged(const uint8_t* bytes, int sz)
{
#if defined (__SSE2__)
    auto sum256 = [](const uint8_t a[], size_t n)
    {
        // constant vector of all zeros
        const __m128i vk0 = _mm_set1_epi8(0);
        // constant vector of all 1's
        const __m128i vk1 = _mm_set1_epi16(1);
        __m128i sum128 = _mm_set1_epi16(0);
        for (size_t i = 0; i < n; i += 16)
        {
            // load 16 bytes of 8 bit values
            __m128i v = _mm_load_si128((const __m128i*)&a[i]);
            // unpack to two vectors in 16 bit values
            __m128i vl = _mm_unpacklo_epi8(v, vk0);
            __m128i vh = _mm_unpackhi_epi8(v, vk0);
            // add vl and vh to sum128
            sum128 = _mm_add_epi16(sum128, _mm_add_epi16(vl, vh));
            //std::cout << "low32=" << std::hex << _mm_cvtsi128_si32(sum128) 
            //<< std::dec << std::endl;
        }
        // now sum128 has 8 sums in 16-bit. Combine them into 4 sums in 32-bit
        sum128 = _mm_madd_epi16(sum128, vk1);
        // horizontal add of 4 sums in 32-bit
        sum128 = _mm_add_epi32(sum128, _mm_srli_si128(sum128, 8));
        sum128 = _mm_add_epi32(sum128, _mm_srli_si128(sum128, 4));
        uint32_t lsum = _mm_cvtsi128_si32(sum128);
        return lsum;
    };
    uint64_t sum {0};
    // add up to 256*16 bytes one time to avoid overflow in sum256
    constexpr int max_add_bytes = 256*16;
    while (sz >= max_add_bytes)
    {
        sum += sum256(bytes, max_add_bytes);
        bytes += max_add_bytes;
        sz -= max_add_bytes;
    }
    if (sz)
    {
        size_t even_sz = (sz/16)*16;
        size_t oddsz = sz - even_sz;
        if (even_sz)
        {
            sum += sum256(bytes, even_sz);
            bytes += even_sz;
            sz -= even_sz;
        }
        while (oddsz)
        {
            sum += *bytes++;
           --oddsz;
        }
    }
    return sum;
#else
    uint64_t sum {0};
    while (sz)
    {
        sum += *bytes++;
        --sz;
    }
    return sum;
#endif
}

uint64_t fastSum(const uint8_t* bytes, int sz)
{
#if defined (__SSE2__)
    uint64_t sum {0};
    // check alignment of bytes
    const uint8_t* aligned_addr = constAlign(bytes, 16);
    while (bytes < aligned_addr)
    {
        sum += *bytes++;
        --sz;   
    }
    return sum + fastSumAliged(bytes, sz);
#else
    uint64_t sum {0};
    while (sz)
    {
        sum += *bytes++;
        --sz;
    }
#endif
}

void fastMemcpyAligned(void *dest, void *src, size_t sz)
{
#if defined (__AVX2__)
    // requies -mavx2
    // assert alighments
    assert(sz % sizeof(__m256i) == 0);
    assert((intptr_t(dest) & (sizeof(__m256i)-1)) == 0);
    assert((intptr_t(src) & (sizeof(__m256i)-1)) == 0);
    __m256i *s = (__m256i*)src;
    __m256i *d = (__m256i*)dest;
    for (size_t n = sz >> 5; n > 0; n--, s++, d++)
        _mm256_stream_si256(d, _mm256_stream_load_si256(s));
    _mm_sfence();
#elif defined (__SSE2__)
    assert(sz % sizeof(__m128i) == 0);
    assert((intptr_t(dest) & (sizeof(__m128i)-1)) == 0);
    assert((intptr_t(src) & (sizeof(__m128i)-1)) == 0);
    __m128i *s = ( __m128i*)src;
    __m128i *d = (__m128i*)dest;
    for (size_t n = sz >> 4; n > 0; n--, s++, d++)
        _mm_stream_si128(d, _mm_stream_load_si128(s));
    _mm_sfence();
#else
    assert(sz % sizeof(uint64_t) == 0);
    assert((intptr_t(dest) & (sizeof(uint64_t)-1)) == 0);
    assert((intptr_t(src) & (sizeof(uint64_t)-1)) == 0);
    for(size_t n_sz = sz >> 3; n_sz > 0 ; --n_sz)
        *(uint64_t*)dest++ = *(uint64_t*)src++;
#endif
}

void fastMemcpyAlignedBackword(void *dest, void *src, size_t sz)
{
#if defined (__AVX2__)
    // assert alighments
    assert(sz % sizeof(__m256i) == 0);
    assert((intptr_t(dest) & (sizeof(__m256i)-1)) == 0);
    assert((intptr_t(src) & (sizeof(__m256i)-1)) == 0);
    size_t n_sz = sz >> 5;
    __m256i *s = (__m256i*)src + n_sz;
    __m256i *d = (__m256i*)dest + n_sz;
    for (; n_sz > 0; n_sz--, --s, --d)
        _mm256_stream_si256(d, _mm256_stream_load_si256(s));
    _mm_sfence();
#elif defined (__SSE2__)
    assert(sz % sizeof(__m128i) == 0);
    assert((intptr_t(dest) & (sizeof(__m128i)-1)) == 0);
    assert((intptr_t(src) & (sizeof(__m128i)-1)) == 0);
    size_t n_sz = sz >> 4;
    __m128i *s = (__m128i*)src + n_sz;
    __m128i *d = (__m128i*)dest + n_sz;
    for (; n_sz > 0; n_sz--, s++, d++)
        _mm_stream_si128(d, _mm_stream_load_si128(s));
    _mm_sfence();
#else
    assert(sz % sizeof(uint64_t) == 0);
    assert((intptr_t(dest) & (sizeof(uint64_t)-1)) == 0);
    assert((intptr_t(src) & (sizeof(uint64_t)-1)) == 0);
    size_t n_sz = sz >> 3;
    const uint64_t* s = (const uint64_t*)src + n_sz;
    uint64_t* d = (uint64_t*)dest + n_sz;
    for(; n_sz > 0 ; --n)
        *(uint64_t*)dest++ = *(uint64_t*)src++;
#endif
}

size_t strHash(const char * str)
{
    size_t result = 0;
    if (ALT_LIKELY(str))
    {
        for (const char*cp =str; *cp; ++cp)
        {
            result = (result << 5) - result + *cp;
        }
    }
    return result;
}

size_t strHash(const char * str, size_t length)
{
    size_t result = 0;
    const char* cp = str;
    while (length-- > 0)
    {
        result = (result << 5) - result + *(cp++);
    }
    return result;
}

size_t strHash (const char *data, size_t len, size_t seed)
{
    unsigned long tmp;
    int rem;
    size_t hash = seed;

    if (len <= 0 || data == nullptr) return 0;

    rem = len & 3;
    len >>= 2;

    // Main loop
    for (; len > 0; len--)
    {
        hash  += *reinterpret_cast<const uint16_t*>(data);
        tmp    = (*reinterpret_cast<const uint16_t*>(data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 4;
        hash  += hash >> 11;
    }

    // Handle end cases
    switch (rem)
    {
        case 3: hash += *reinterpret_cast<const uint16_t*>(data);
                hash ^= hash << 16;
                hash ^= data[2] << 18;
                hash += hash >> 11;
                break;
        case 2: hash += *reinterpret_cast<const uint16_t*>(data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += *data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    // Force "avalanching" of final 127 bits
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;
    return hash;
}

size_t strSplit (
    const char * str,
    std::vector<std::string>& substrings,
    size_t length,
    char separator,
    char terminator,
    bool skip_leading_sp,
    bool skip_trailing_sp)
{
    if (!str) return 0;

    if (length==0)
    {
        length = std::strlen(str); // fastStrLen(str);
    }

	size_t scanned(0);
    size_t pos = 0;
	size_t start_pos(pos);
	size_t end_pos(pos);
	bool string_started (false);
	while (pos < length && str[pos]!=terminator)
	{
	    if (str[pos]==separator)
		{
			substrings.emplace_back(str+start_pos, end_pos-start_pos);
			string_started = false;
			++scanned;
			start_pos = pos+1;
			end_pos = start_pos;
		}
		else if (isspace(str[pos]))
		{
			if (skip_leading_sp && !string_started)
			{
				++start_pos;
				++end_pos;
			}
			else if (!skip_trailing_sp  && string_started)
			{
				++end_pos;
			}   
		}
		else
		{
			string_started = true;
			++end_pos;
		}
		++pos;
  	}
	if (end_pos>start_pos)
	{
	    substrings.emplace_back(str+start_pos, end_pos-start_pos);
		++scanned;
	}
	return scanned;
}

size_t strSplitQuoted (
    const char * str,
    std::vector<std::string>& substrings,
    char separator,
    bool skip_leading_sp,
    bool skip_trailing_sp)
{
    char terminator = '\0';
    if (*str=='(') { terminator = ')'; ++str; }
    else if (*str=='{') { terminator = '}'; ++str; }
    else if (*str=='[') { terminator = ']'; ++str;  }
    else if (*str=='\'') { terminator = '\''; ++str;  }
    else if (*str=='"') { terminator = '"'; ++str;  }
    size_t parsed = strSplit(
        str, substrings, 0, separator,terminator,skip_leading_sp,skip_trailing_sp);
    if (terminator)
    {
        if (*(str+parsed)==terminator) ++parsed;
        ++parsed;
    } 
    
    return parsed;
}

char hexDigit (char ch)
{
    uint8_t diff;
    if ((diff = uint8_t(ch - '0')) < 10)
    {
        return diff;
    }
    else if ((diff = uint8_t(ch - 'A')) < 6)
    {
        return 10 + diff;
    }
    else if ((diff = uint8_t(ch - 'a')) < 6)
    {
        return 10 + diff;
    }
    else
    {
        return INVALID_HEX_DIGIT;
    }
};

size_t scanUTF8String(const char * utf8Char, char32_t& wch)
{
    //std::cout << "scanUTF8String " << std::hex << int(uint8_t(*utf8Char)) << std::dec << std::endl;
    if (!utf8Char || *utf8Char==0)
    {
        wch = char32_t(0);
        return 0;
    }
    char ch = *utf8Char;
    if ((ch & 0x80)==0)
    {
        wch = char32_t(ch);
        return 1;
    }

    size_t bytes = uCharBytes(ch);
    //std::cout << "CHAR BYTES " << bytes << std::endl;
    wch = char32_t(uint8_t(~(uint16_t(0xFF00) >> bytes)) & uint8_t(ch));
    bool is_valid_uft8{true};
    constexpr char32_t MAX_UTF8_CODE = 0x10FFFF;
    static std::vector<std::pair<char32_t,char32_t>> utf8_ranges
        { {0, 0X80}, {1, 0X780}, {1, 0XF800}, {1, MAX_UTF8_CODE-0XFFFF} };
    for (size_t b=1; b<bytes; ++b)
    {
        char byte = *(utf8Char+b);
        if ((byte & 0xC0) != 0x80)
        {
            is_valid_uft8 = false;
            break;
        }
        byte &=  0x3F;
        wch = (wch << 6) | byte;
        if (wch < utf8_ranges[b].first || wch > utf8_ranges[b].second)
        {
            // std::cout << "INVALID CHAR BYTES b=" << b << " byte=" << int(uint8_t(byte)) << std::endl;
            is_valid_uft8 = false;
            break;   
        }
    }

    if (!is_valid_uft8)
    {
        wch = char32_t('?');
    }

    return bytes;
}

} // namespace alt

#if 0
int main ()
{
    uint8_t ints[777];
    for (size_t i=0; i<sizeof(ints); ++i) ints[i]=1;
    auto sum = alt::fastSum(ints, sizeof(ints));
    std::cout << "SUM=" << sum << std::endl;

    const char * str = "snch whissw n iwdksllsfhhw sdd ssfff fxxxxxxxxxdq";
    const char * cp = alt::fastStrChr(str, 'q');
    const char * cp1 = strchr(str, 'q');
    size_t len = alt::fastStrLen(str);
    if (cp && cp1)
        std::cout << "FAST FIND pos=" << (int)(cp-str)
        << " pos=" << (int)(cp1-str)
        << std::endl;
    else
    {
        std::cout << "Not FIND" << std::endl;
    }
    std::cout << "FAST LEN=" << len << " len=" << strlen(str) << std::endl;
    
    return 0;
}
#endif