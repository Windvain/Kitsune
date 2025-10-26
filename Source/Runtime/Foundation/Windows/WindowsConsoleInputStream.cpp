#include "Foundation/Logging/ConsoleInputStream.h"
#include <Windows.h>

namespace Kitsune
{
    void ConsoleInputStream::ReadFromConsole()
    {
        DWORD countRead;
        ::ReadFile(::GetStdHandle(STD_INPUT_HANDLE), m_Buffer, static_cast<DWORD>(s_BufferSize),
                   &countRead, nullptr);

        m_Pointer = m_Buffer + countRead;
    }
}
