#include "Foundation/IO/ConsoleOutputStream.h"

#include <cstring>
#include "Foundation/String/StringView.h"

#include "Foundation/String/Valid.h"
#include "Foundation/String/Utf8Encoding.h"

#include "Foundation/Threading/LockGuard.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    void ConsoleOutputStream::Write(const char* data, Usize count)
    {
        LockGuard guard_(m_Lock);
        StringView dataView(data, count);

        while (!dataView.IsEmpty())
        {
            *m_Pointer = dataView.Front();
            ++m_Pointer;

            if ((m_Pointer == (m_Buffer + s_BufferSize)) || (dataView.Front() == '\0') ||
                (dataView.Front() == '\n'))
            {
                ThreadUnsafeFlush_();
            }

            dataView.RemovePrefix(1);
        }
    }

    void ConsoleOutputStream::Flush()
    {
        LockGuard guard_(m_Lock);
        ThreadUnsafeFlush_();
    }

    void ConsoleOutputStream::ThreadUnsafeFlush_()
    {
        const char* modified = FindInvalidEncoding<Utf8Encoding<char>>(m_Buffer, m_Pointer);
        if ((m_Pointer != m_Buffer) && (modified == m_Buffer))
        {
            const char* replacementChar = "\uFFFD";
            WriteToConsole_(replacementChar, replacementChar + 3);

            m_Pointer = m_Buffer;
            throw InvalidArgumentException("Tried to write an invalid string to the console.");
        }

        WriteToConsole_(m_Buffer, modified);

        if (modified != m_Pointer)
        {
            Ptrdiff charsRemaining = m_Buffer + s_BufferSize - modified;
            std::memmove(m_Buffer, modified, charsRemaining * sizeof(char));

            m_Pointer = m_Buffer + charsRemaining;
            return;
        }

        m_Pointer = m_Buffer;
    }
}
