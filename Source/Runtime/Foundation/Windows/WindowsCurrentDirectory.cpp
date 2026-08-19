#include "Foundation/Filesystem/CurrentDirectory.h"
#include <Windows.h>

#include "Foundation/Threading/LockGuard.h"
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/SystemException.h"

#undef SetCurrentDirectory
#undef GetCurrentDirectory

namespace Kitsune::Filesystem
{
    // Put this in an inline namespace, make this inaccessible to other files.
    namespace
    {
        Mutex g_CurrentDirectoryMutex{ /* ... */ };
    }

    void SetCurrentDirectory(PathView path)
    {
        LockGuard lock(g_CurrentDirectoryMutex);

        NativeString nativePath = path.Native();
        BOOL success = ::SetCurrentDirectoryW(nativePath.Raw());

        if (!success)
        {
            throw SystemException(
                "Failed to set the current directory of the process. This is probably "
                "due to the specified path being invalid.");
        }
    }

    Path GetCurrentDirectory()
    {
        LockGuard lock(g_CurrentDirectoryMutex);

        // https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcurrentdirectory
        // The documentation is incorrect. GetCurrentDirectory() returns the length
        // INCLUDING the null terminator.
        DWORD length = ::GetCurrentDirectoryW(0, nullptr);
        if (length == 0)
            throw SystemException("Failed to get the current directory path.");

        WideString currentDir(length - 1, L'\0');
        ::GetCurrentDirectoryW(length, currentDir.Data());

        return TranscodeString<UTF16Encoding<wchar_t>, UTF8Encoding<char>>(currentDir);
    }
}
