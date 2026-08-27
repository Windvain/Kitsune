#include "Foundation/Filesystem/ExecutablePath.h"
#include <Windows.h>

#include "Foundation/String/TranscodePresets.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune::Filesystem
{
    Path GetExecutablePath()
    {
        // HACK: GetModuleFileNameW() won't return the **exact** size needed to fit the
        // executable path, so this code needs to "guess" it.
        WideString path(MAX_PATH, L'\0');
        do
        {
            if (::GetModuleFileNameW(nullptr, path.Data(), DWORD(path.Size() + 1)) == 0)
                throw SystemException("Failed to get the executable's path.");

            path.Resize(path.Size() * 2);
        }
        while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

        // Recalculate the size of the path, then return it as a UTF-8 encoded string.
        return UTF16ToUTF8<wchar_t, char>(path.Data());
    }
}
