#include "Foundation/IO/ConsoleOutputStream.h"
#include "Foundation/Threading/LockGuard.h"

namespace Kitsune
{
    void ConsoleOutputStream::Write(const char* data, Usize count)
    {
        LockGuard guard_(m_Lock);
        for (; count > 0; --count, ++data)
        {
            *m_Pointer = *data;
            ++m_Pointer;

            if ((m_Pointer == (m_Buffer + s_BufferSize)) ||
                (*data == '\0') || (*data == '\n'))
            {
                ThreadUnsafeFlush();
            }
        }
    }

    void ConsoleOutputStream::Flush()
    {
        LockGuard guard_(m_Lock);
        ThreadUnsafeFlush();
    }
}
