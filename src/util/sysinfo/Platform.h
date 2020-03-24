
#pragma once

// Platform Operating System
// OS 
#define ALT_OS_UNKNOWN     0
#define ALT_OS_LINUX       1
#define ALT_OS_WINDOWS     2
#define ALT_OS_MAC_OS_X    3 // For iOS and OSX
#define ALT_OS_ANDRIOD     4

// OS FAMILY
#define  ALT_OS_FAMILY_UNKNOWN  0
#define  ALT_OS_FAMILY_LINUX    1
#define  ALT_OS_FAMILY_BSD      2
#define  ALT_OS_FMAIL_WINDOWS   3

#if defined (__ANDRIOD__)
    #define ALT_OS          ALT_OS_ANDRIOD
    #define ALT_OS_FAMILY   ALT_OS_FAMILY_LINUX
#elif defined (__linux__)
    #define ALT_OS          ALT_OS_LINUX
    #define ALT_OS_FAMILY   ALT_OS_FAMILY_LINUX
#elif defined (__CYGWIN__)
    #if defined (_GNU_SOURCE)
        #define ALT_OS          ALT_OS_LINUX
        #define ALT_OS_FAMILY   ALT_OS_FAMILY_LINUX
    #else
        #define ALT_OS          ALT_OS_WINDOWS
        #define ALT_OS_FAMILY   ALT_OS_FAMILY_WINDOWS
    #endif
#elif defined (__APPLE__)
    #define ALT_OS          ALT_OS_MAC_OS_X
    #define ALT_OS_FAMILY   ALT_OS_FAMILY_MAC
#elif defined (_WIN32) || defined (_WIN64)
    #define ALT_OS          ALT_OS_WINDOWS
    #define ALT_OS_FAMILY   ALT_OS_FAMILY_WINDOWS
#else
    #error "OS unknown, not supported"
#endif

// OS depednet marcos
#if (ALT_OS_FAMILY==ALT_OS_WINDOWS)
    #define ALT_NEWLINE    "\r\n"
#else
    #define ALT_NEWLINE    "\n"
#endif

// Platform CPU Architectures
#define ALT_ARCH_UNKNOWN   0
#define ALT_ARCH_I386      1  // 32 Bit X86
#define ALT_ARCH_AMD64     2  // X86-64 by Intel
#define ALT_ARCH_ARM       3  // 32 Bit RISC
#define ALT_ARCH_AARCH64   4  // 64 Bit RISC, ARM64 by appple
#define ALT_ARCH_RISCV64   5  // 64 Bit RISC-V by OSS

// gcc -march=native -dM -E - </dev/null 
#if defined (__x86_64__) || defined (_X86_) || defined (__amd64__)
    #define ALT_CPU        ALT_ARCH_AMD64
#elif defined (__arm__)
    #define ALT_CPU        ALT_ARCH_ARM
#elif defined (__aarch64__)
    #define ALT_CPU        ALT_ARCH_ARM64
#elif defined (__powerpc64__)
    #define ALT_CPU        ALT_ARCH_POWERPC
#elif defined (__aarch64__)
    #define ALT_CPU        ALT_ARCH_AARCH64
#elif defined (__riscv) && (__riscv_xlen==64)
    #define ALT_CPU        ALT_ARCH_RISCV64
#elif defined (__i386__) || defined (i386) || defined (__i386)
    #define ALT_CPU        ALT_ARCH_I386
#else
    #define ALT_CPU        ALT_ARCH_UNKNOWN
    #error "Unknown architecture"
#endif

// Compiler, currently only GCC 
#define ALT_COMPILER_UNKNOWN    0
#define ALT_COMPILER_GCC        1

#if defined (__GNUC__)
    #define ALT_COMPLIER     ALT_COMPILER_GCC
#else
    #error "Compiler not supported"
#endif

// C++ attributes
#if (__cplusplus >= 2017003L)
    // C++17
    #define ALT_FALLTROUGH        [[fallthrough]]
    #define ALT_NODISCARD         [[nodiscard]]
    #define ALT_MAYBE_UNUSED      [[maybe_unused]]
#endif
#if (__cplusplus >= 201402L)
    // C++ 14
    #define ALT_DEPRECATED        [[deprecated]]
    #define ALT_DEPRECATED_BECUASE(reason) [[deprecated(reason)]]
#endif
#if (__cplusplus >= 201103L)
    // C++ 11
    #define ALT_NORETURN            [[noreturn]]
    #define ALT_CARRIES_DEPENDENCY  [[carries_dependency]]
#else
    #error "Requires C++11 or later"
#endif
// Compiler specific attributes
//#if (ALT_COMPILER==ALT_COMPILER_GCC)
#if defined (__GNUC__)
    // inline
    #if defined(NDEBUG)
        #define ALT_INLINE        [[gnu::always_inline]]
    #else
        #define ALT_INLINE inline  // Do not force inline on debug
    #endif
    #define ALT_HOT               [[gnu::hot]]
    #define ALT_COLD              [[gnu::cold]]
    #define ALT_LIKELY(x)         __builtin_expect(!!(x), 1)
    #define ALT_UNLIKELY(x)       __builtin_expect(!!(x), 0)
    #ifndef ALT_DEPRECATED
        #define ALT_DEPRECATED    [[gnu::deprecated]]
        #define ALT_DEPRECATED_BECUASE(reason)  [[gnu::deprecated(reason)]]
    #endif
    #ifndef ALT_NODISCARD
        #define ALT_NODISCARD     [[gnu::warn_unused_result]]
    #endif
#endif
// Define default for all undefined attributes
#ifndef ALT_FALLTROUGH
    #define ALT_FALLTROUGH
#endif
#ifndef ALT_NODISCARD
    #define ALT_NODISCARD
#endif
#ifndef ALT_MAYBE_UNUSED
    #define ALT_MAYBE_UNUSED
#endif
#ifndef ALT_DEPRECATED
    #define ALT_DEPRECATED
    #define ALT_DEPRECATED_BECUASE(x)
#endif
#ifndef ALT_INLINE
    #define ALT_INLINE    inline
#endif
#ifndef ALT_HOT
    #define ALT_HOT
#endif
#ifndef ALT_COLD
    #define ALT_COLD
#endif
#ifndef ALT_LIKELY
    #define ALT_LIKELY(x)     x
    #define ALT_UNLIKELY(x)   x
#endif

#if defined _WIN32 || defined __CYGWIN__
  #define ALT_IMPORT __declspec(dllimport)
  #define ALT_EXPORT __declspec(dllexport)
  #define ALT_LOCAL
#else
  #if __GNUC__ >= 4
    #define ALT_IMPORT __attribute__ ((visibility ("default")))
    #define ALT_EXPORT __attribute__ ((visibility ("default")))
    #define ALT_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define ALT_IMPORT
    #define ALT_EXPORT
    #define ALT_LOCAL
  #endif
#endif

#if defined _WIN32 || defined __CYGWIN__
#define ALIGNMENT(x)  __declspec(align(x))
#elif defined( __GNUC__ )
#define ALIGNMENT(x)  __attribute__((align(x)))
#endif

#ifdef ALT_CORE_DLL
  // defined when ALT_CORE is compiled as a DLL
  #ifdef ALT_CORE_EXPORTS
    // defined if we are building the TF CORE DLL (instead of using it)
    #define ALT_CORE_PUBLIC ALT_EXPORT
  #else
    #define ALT_CORE_PUBLIC ALT_IMPORT
  #endif
  #define ALT_CORE_LOCAL ALT_LOCAL
#else 
  // when ALT_CORE is compiled as a stati clib
  #define ALT_CORE_PUBLIC
  #define ALT_CORE_LOCAL
#endif

// endian
#define ALT_ENDIAN_UNKNOWN 0
#define ALT_ENDIAN_LITTLE  1
#define ALT_ENDIAN_BIG     2

// First, use library provided by the complier if any
#if defined (__GLIBC__)
    #include <endian.h>
    #if (__BYTE_ORDER==__LITTLE_ENDIAN)
        #define ALT_ENDIAN    ALT_ENDIAN_LITTLE
    #elif (__BYTE_ORDER==_BIG_ENDIAN)
        #define ALT_ENDIAN    ALT_ENDIAN_BIG
    #endif
#endif
// Then, check if there is built-in macro or flag
#if not defined (ALT_ENDIAN)
    #if defined(_BIG_ENDIAN) && !defined(_LITTLE_ENDIAN)
        #define ALT_ENDIAN   ALT_ENDIAN_BIG
    #elif defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
        #define ALT_ENDIAN   ALT_ENDIAN_LITTLE
    #elif defined(__BYTE_ORDER__)
        #if (__BYTE_ORDER__==__ORDER_LITTLE_ENDIAN__)
            #define ALT_ENDIAN  ALT_ENDIAN_LITTLE
        #elif (__BYTE_ORDER__==__ORDER_BIG_ENDIAN__)
            #define ALT_ENDIAN  ALT_ENDIAN_BIG
        #endif
    #endif
#endif
// Finally, check the architecture
#if not defined (ALT_ENDIAN)
    #if (ALT_CPU==ALT_ARCH_I386 || \
         ALT_CPU==ALT_ARCH_AMD64 || \
         ALT_CPU==ALT_RISCV64 \
        )
        #define ALT_ENDIAN   ALT_ENDIAN_LITTLE
    #elif (ALT_CPU==ALT_ARCH_POWERPC || \
           ALT_CPU==ALT_ARCH_AARCH64 \
          )
        #define ALT_ENDIAN   ALT_ENDIAN_BIG
    #elif (ALT_CPU==ALT_ARCH_ARM || \
           ALT_CPU==ALT_ARCH_ARM64 \
          )
        #if defined(__ARMEB__)
            #define ALT_ENDIAN   ALT_ENDIAN_BIG
        #else
            #define ALT_ENDIAN   ALT_ENDIAN_LITTLE
        #endif
    #elif (ALT_CPU==ALT_ARCH_AARCH64)
        #if defined(__AARCH64EB__)
            #define ALT_ENDIAN   ALT_ENDIAN_BIG
        #elif deinfed(__AARCH64EL_)
            #define ALT_ENDIAN   ALT_ENDIAN_LITTLE
        #else
            #error "Unknown Edian in AARCH64"
        #endif
    #else
        #error "Unknown Endian by architecture"
    #endif
#endif

#define CHAR_BIT       8

