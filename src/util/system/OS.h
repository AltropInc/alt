#pragma once

#include "Platform.h"

#if (ALT_OS==ALT_OS_WINDOWS)
#include <shlobj.h>         // for LoadLibraryA used in dll_open
#else
#include <dlfcn.h>          // for dlopen used in dll_open
#endif

#include <filesystem>       // std::filesystem::path
#include <iostream>         // For std::cout

namespace alt
{
//---------------------------------------------------------------------------------------
// File Descriptor
//---------------------------------------------------------------------------------------
#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)
using FdId     = UINT_PTR;
using SocketId = SOCKET;
constexpr SocketId INVALID_SOCK_ID = INVALID_SOCKET;
#else
using FdId     = int;
using SocketId = int;
constexpr SocketId INVALID_SOCK_ID = -1;
#endif

//---------------------------------------------------------------------------------------
// Application Data Path
//---------------------------------------------------------------------------------------
std::string getAppDataPath(bool for_this_user_only=true);

//---------------------------------------------------------------------------------------
// Dynamic libraries
//---------------------------------------------------------------------------------------
#if (ALT_OS_FAMILY==ALT_OS_FAMILY_WINDOWS)

typedef HINSTANCE dll_handle;
constexpr dll_handle null_dll_handle = 0;

inline dll_handle dll_open(const std::filesystem::path & path)
{
    return LoadLibraryA(path.string().c_str);
}

template <typename ST>
inline ST dll_sym (dll_handle handle, char* symbol)
{
	return (ST) GetProcAddress (handle, symbol);
}

inline void dll_close (dll_handle handle)
{
	FreeLibrary (handle);
}

#else // for ALT_OS == ALT_OS_LINUX, ALT_OS_OSX, or ALT_OS_ANDRIOD)

typedef void* dll_handle;
constexpr dll_handle null_dll_handle = nullptr;

inline dll_handle dll_open(const std::filesystem::path & path)
{
    return dlopen(path.string().c_str(), RTLD_LAZY);
}

template <typename ST>
inline ST dll_sym (dll_handle handle, char* symbol)
{
	return (ST) dlsym(handle, symbol);
}

inline void dll_close (dll_handle handle)
{
	dlclose(handle);
}

#endif

}


