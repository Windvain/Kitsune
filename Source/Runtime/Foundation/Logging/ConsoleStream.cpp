#include "Foundation/Logging/ConsoleStream.h"

namespace Kitsune
{
    void ConsoleOutputStream::Write(const char* ptr, Usize count)
    {
        for (; count > 0; --count, ++ptr)
        {
            *m_Buffer.GetCurrent() = *ptr;
            m_Buffer.BumpPointer(1);

            if ((*ptr == '\n') || (*ptr == '\0') ||
                (m_Buffer.GetRemainingCapacity() == 0))
            {
                Overflow();
            }
        }
    }

    void ConsoleInputStream::Read(Usize count)
    {
        while (count > 0)
        {
            Underflow();

            Usize min = KITSUNE_MIN(count, Usize(m_Buffer.GetWrittenCount()));
            m_OutStream.Write(m_Buffer.GetBegin(), min);

            count -= min;
        }
    }

    void ConsoleInputStream::Read()
    {
        do
        {
            Underflow();
            m_OutStream.Write(m_Buffer.GetBegin(), m_Buffer.GetWrittenCount());
        }
        while (m_Buffer.Back() != '\n');
    }
}
