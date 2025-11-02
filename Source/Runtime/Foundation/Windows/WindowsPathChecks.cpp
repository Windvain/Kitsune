#include "Foundation/Filesystem/PathChecks.h"
#include <Windows.h>

namespace Kitsune::Filesystem
{
    KITSUNE_FORCEINLINE bool IsWindowsSeperator(wchar_t ch)
    {
        return ((ch == '\\') || (ch == '/'));
    }

    bool PathExists(const Path& str)
    {
        DWORD attributes = ::GetFileAttributesW(str.Raw());
        return (attributes != INVALID_FILE_ATTRIBUTES);
    }

    bool IsFile(const Path& str)
    {
        DWORD attributes = ::GetFileAttributesW(str.Raw());
        return ((attributes != INVALID_FILE_ATTRIBUTES) &&
               !(attributes &  FILE_ATTRIBUTE_DIRECTORY));
    }

    bool IsDirectory(const Path& str)
    {
        DWORD attributes = ::GetFileAttributesW(str.Raw());
        return ((attributes != INVALID_FILE_ATTRIBUTES) &&
                (attributes &  FILE_ATTRIBUTE_DIRECTORY));
    }
}
