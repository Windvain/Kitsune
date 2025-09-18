#include "Foundation/Logging/ConsoleOutputStream.h"
#include <Windows.h>

namespace Kitsune
{
    void ConsoleOutputStream::WriteToConsole(const NativeChar* begin,
                                             const NativeChar* end)
    {
        ::WriteConsoleW(::GetStdHandle(STD_OUTPUT_HANDLE),
                        begin, DWORD(end - begin),
                        nullptr, nullptr);

        m_Pointer = m_Buffer;
    }
}
