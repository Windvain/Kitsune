#include "Foundation/Filesystem/CurrentDirectory.h"
#include <Windows.h>

#include "Foundation/Threading/LockGuard.h"
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/SystemException.h"

#undef SetCurrentDirectory
#undef GetCurrentDirectory

namespace Kitsune
{
    Mutex g_CurrentDirectoryMutex{ /* ... */ };

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

        DWORD length = ::GetCurrentDirectoryW(0, nullptr);
        if (length == 0)
            throw SystemException("Failed to get the current directory path.");

        WideString currentDir(length, L'\0');
        ::GetCurrentDirectoryW(length + 1, currentDir.Data());

        return TranscodeString<UTF16Encoding<wchar_t>, UTF8Encoding<char>>(currentDir);
    }
}
