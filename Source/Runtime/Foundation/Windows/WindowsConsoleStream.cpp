#include "Foundation/IO/ConsoleOutputStream.h"
#include <Windows.h>

#include "Foundation/String/TranscodePresets.h"

namespace Kitsune
{
    void ConsoleOutputStream::WriteToConsole_(const char* begin, const char* end)
    {
        HANDLE outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if ((outputHandle == nullptr) || (outputHandle == INVALID_HANDLE_VALUE))
            return;

        StringView string(begin, end);
        auto wideString = Utf8ToUtf16<char, wchar_t>(string);

        ::WriteConsoleW(outputHandle, wideString.Raw(),
                        static_cast<DWORD>(wideString.Size()),
                        nullptr, nullptr);
    }
}
