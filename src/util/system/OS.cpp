#include "OS.h"

#if (ALT_UNDERLYING_OS==ALT_OS_WINDOWS)
#include <windows.h>
#include <Shlobj.h>
#else
#include <dlfcn.h>   // for dlopen used in dll_open
#endif

#if (ALT_UNDERLYING_OS==ALT_OS_LINUX)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#if (ALT_UNDERLYING_OS==ALT_OS_OSX)
#include <NSSystemDirectories.h>
#import <Foundation/Foundation.h>
#endif

namespace alt {

std::string getAppDataPath(bool for_this_user_only)
{
#if (ALT_UNDERLYING_OS==ALT_OS_WINDOWS)
    char path[MAX_PATH];
    int folder_id = for_this_user_only ? CSIDL_APPDATA : CSIDL_COMMON_APPDATA;
    if ( SUCCEEDED( SHGetFolderPath( NULL, folder_id, NULL, 0, path ) ) )
    {
        //std::wstring_convert<std::codecvt_utf8_utf16<TCHAR>,TCHAR> conversion;
        //std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conversion;
        //return conversion.to_bytes(path);
        return path;
    }
    return std::string();
#endif
#if (ALT_UNDERLYING_OS==ALT_OS_LINUX)
    // linux does not have a concept for application data. There is no writable folder
    // for common application data. Use user's home directory for user specific data
    const char *homedir;
    if ((homedir = getenv("HOME")) == nullptr)
    {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return homedir;
#endif
#if (ALT_UNDERLYING_OS==ALT_OS_OSX)
    // https://stackoverflow.com/questions/36634632/accessing-standard-directories-on-os-x-with-c
    NSSearchPathDomainMask domainMask = for_this_user_only ? NSUserDomainMask  : NSLocalDomainMask;
    char path[PATH_MAX];
    NSSearchPathEnumerationState state =
        NSStartSearchPathEnumeration(NSApplicationSupportDirectory, domainMask);
    while (state = NSGetNextSearchPathEnumeration(state, path))
    {
        return path;
    }
    return std::string();
#endif
#if (ALT_UNDERLYING_OS==ALT_OS_ANDRIOD)
    return "/data/data";
#endif
}

}