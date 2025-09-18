#include "Foundation/Logging/ConsoleOutputStream.h"

#include "Foundation/String/String.h"
#include "Foundation/Threading/LockGuard.h"

#include "Foundation/String/UnicodeConversion.h"

namespace Kitsune
{
    ConsoleOutputStream::~ConsoleOutputStream()
    {
        Flush();
    }

    void ConsoleOutputStream::Write(const char* data, Usize count)
    {
        LockGuard guard_(m_Lock);
        for (; count > 0; --count, ++data)
        {
            *m_Pointer = *data;
            ++m_Pointer;

            if ((m_Pointer == (m_Buffer + s_BufferSize)) ||
                (*data == '\n') || (*data == '\0'))
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

    void ConsoleOutputStream::ThreadUnsafeFlush()
    {
        // Can't use "if constexpr (std::is_same_v<NativeChar, char>)" here, compiler error.
#if !defined(KITSUNE_OS_WINDOWS)
        WriteToConsole(m_Buffer, m_Pointer);
#else
        NativeString convString;
        Unicode::Convert(m_Buffer, m_Pointer, BackInsertIterator(convString));

        WriteToConsole(convString.GetBegin(), convString.GetEnd());
#endif
    }
}
