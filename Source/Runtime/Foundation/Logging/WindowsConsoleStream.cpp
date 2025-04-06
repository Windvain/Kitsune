#include "Foundation/Logging/ConsoleStream.h"

#include <Windows.h>
#include "Foundation/String/StringView.h"

namespace Kitsune
{
    inline StringView GetValidUtf8Segment(const char* begin, const char* guess)
    {
        --guess;

        for (; ((*guess & 0x80) != 0) && ((*guess & 0xC0) != 0xC0); --guess);
        return StringView(begin, guess + 1);
    }

    void ConsoleOutputStream::Overflow()
    {
        constexpr Ptrdiff WideBufSize = 128;

        // STD_OUTPUT_HANDLE (-11) --> STD_ERROR_HANDLE (-12)
        HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE - DWORD(m_ErrorStream));
        wchar_t convBuffer[WideBufSize];

        char* begin = m_Buffer.GetBegin();
        while (begin != m_Buffer.GetCurrent())
        {
            Ptrdiff min = KITSUNE_MIN(WideBufSize, m_Buffer.GetCurrent() - begin);
            StringView str = GetValidUtf8Segment(begin, begin + min);

            int wideLength = ::MultiByteToWideChar(CP_UTF8, 0, begin, static_cast<int>(str.Size()),
                                                   convBuffer, WideBufSize);

            if (wideLength == 0) [[unlikely]]
                break;

            begin += str.Size();
            ::WriteConsoleW(handle, convBuffer, DWORD(wideLength), nullptr, 0);
        }

        m_Buffer.SetPointer(m_Buffer.GetBegin());
    }

    void ConsoleInputStream::Underflow()
    {
        HANDLE handle = ::GetStdHandle(STD_INPUT_HANDLE);
        DWORD countRead;

        ::ReadFile(handle, m_Buffer.GetBegin(), m_Buffer.GetSize(), &countRead, nullptr);
        m_Buffer.SetPointer(m_Buffer.GetBegin() + countRead);
    }
}
