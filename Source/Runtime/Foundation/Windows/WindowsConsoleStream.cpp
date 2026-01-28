#include "Foundation/IO/ConsoleOutputStream.h"
#include <Windows.h>

#include "Foundation/String/Transcode.h"

#include "Foundation/String/Utf8Encoding.h"
#include "Foundation/String/Utf16Encoding.h"

namespace Kitsune
{
    void ConsoleOutputStream::ThreadUnsafeFlush()
    {
        HANDLE outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if ((outputHandle == nullptr) || (outputHandle == INVALID_HANDLE_VALUE))
            return;

        StringView string(m_Buffer, m_Pointer);
        auto wideString = Transcode<Utf8Encoding<char>, Utf16Encoding<wchar_t>>(string);

        ::WriteConsoleW(outputHandle, wideString.Raw(), static_cast<DWORD>(wideString.Size()),
                        nullptr, nullptr);

        m_Pointer = m_Buffer;
    }
}
