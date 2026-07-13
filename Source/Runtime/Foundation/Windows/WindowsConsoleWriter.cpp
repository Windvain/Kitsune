#include "Foundation/Streams/ConsoleWriter.h"
#include <Windows.h>

#include "Foundation/String/String.h"
#include "Foundation/String/TranscodePresets.h"

namespace Kitsune::Details
{
    void UnbufferedWriteConsole(ConsoleWriterType streamType, StringView string)
    {
        DWORD handleType = (streamType == ConsoleWriterType::StdOut) ?
            STD_OUTPUT_HANDLE : STD_ERROR_HANDLE;

        HANDLE handle = ::GetStdHandle(handleType);
        if ((handle == nullptr) || (handle == INVALID_HANDLE_VALUE))
            return;

        WideString wideString = UTF8ToUTF16<char, wchar_t>(string);
        ::WriteConsoleW(
            handle, wideString.Data(), static_cast<DWORD>(wideString.Size()),
            nullptr, nullptr);
    }
}
