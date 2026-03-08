#include "Foundation/IO/ConsoleOutputStream.h"
#include <Windows.h>

#include "Foundation/String/TranscodePresets.h"

namespace Kitsune
{
    void ConsoleOutputStream::ThreadUnsafeFlush_()
    {
        HANDLE outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if ((outputHandle == nullptr) || (outputHandle == INVALID_HANDLE_VALUE))
            return;

        StringView string(m_Buffer, m_Pointer);
        auto wideString = Utf8ToUtf16<char, wchar_t>(string);

        ::WriteConsoleW(outputHandle, wideString.Raw(),
                        static_cast<DWORD>(wideString.Size()),
                        nullptr, nullptr);

        m_Pointer = m_Buffer;
    }
}
