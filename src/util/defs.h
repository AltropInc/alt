#pragma once

#ifdef ALT_UTIL_DLL
  // defined when ALT_UTIL is compiled as a DLL
  #ifdef ALT_UTIL_EXPORTS
    // defined if we are building the TF UTIL DLL (instead of using it)
    #define ALT_UTIL_PUBLIC ALT_EXPORT
  #else
    #define ALT_UTIL_PUBLIC ALT_IMPORT
  #endif
  #define ALT_UTIL_LOCAL ALT_LOCAL
#else 
  // when ALT_UTIL is compiled as a stati clib
  #define ALT_UTIL_PUBLIC
  #define ALT_UTIL_LOCAL
#endif