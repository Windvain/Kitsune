#include "Foundation/Filesystem/ToAbsolute.h"

#include <Windows.h>
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune::Filesystem
{
    Path ToAbsolute(PathView path)
    {
        WideString widePath = path.Native();
        DWORD length = ::GetFullPathNameW(widePath.Raw(), 0, nullptr, nullptr);

        if (length == 0)
        {
            throw SystemException(
                "Failed to convert the path from a relative "
                "path to an absolute path.");
        }

        WideString tempBuffer(length - 1, '\0');
        ::GetFullPathNameW(widePath.Data(), length, tempBuffer.Data(), nullptr);

        // HACK: GetFullPathNameW() doesn't return the **exact** length of the string,
        // rather just an estimate that fits the string.
        // Recalculate the size before returning.
        WideString absolutePath(tempBuffer.Data());
        return UTF16ToUTF8<wchar_t, char>(absolutePath);
    }
}
