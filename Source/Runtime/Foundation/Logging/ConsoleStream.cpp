#include "Foundation/Logging/ConsoleStream.h"

namespace Kitsune
{
    void ConsoleOutputStream::Write(const char* ptr, Usize count)
    {
        for (; count > 0; --count, ++ptr)
        {
            *m_StreamBuffer.GetCurrent() = *ptr;
            m_StreamBuffer.BumpPointer(1);

            if ((*ptr == '\n') || (*ptr == '\0') ||
                (m_StreamBuffer.GetRemainingCapacity() == 0))
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

            Usize min = KITSUNE_MIN(count, Usize(m_StreamBuffer.GetWrittenCount()));
            m_OutStream.Write(m_StreamBuffer.GetBegin(), min);

            count -= min;
        }
    }

    void ConsoleInputStream::Read()
    {
        do
        {
            Underflow();
            m_OutStream.Write(m_StreamBuffer.GetBegin(), m_StreamBuffer.GetWrittenCount());
        }
        while (m_StreamBuffer.Back() != '\n');
    }
}
