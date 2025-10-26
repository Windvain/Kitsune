#include "Foundation/Logging/ConsoleOutputStream.h"
#include <Windows.h>

namespace Kitsune
{
    void ConsoleOutputStream::WriteToConsole(const NativeChar* begin,
                                             const NativeChar* end)
    {
        HANDLE outputHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
        if ((outputHandle == nullptr) || (outputHandle == INVALID_HANDLE_VALUE))
            return;

        ::WriteConsoleW(outputHandle,
                        begin, DWORD(end - begin),
                        nullptr, nullptr);

        m_Pointer = m_Buffer;
    }
}
