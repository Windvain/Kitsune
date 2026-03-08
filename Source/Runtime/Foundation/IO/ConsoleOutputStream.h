#pragma once

#include "Foundation/IO/Stream.h"
#include "Foundation/Threading/Mutex.h"

namespace Kitsune
{
    // A stream class that outputs to the console window, if one exists.
    class ConsoleOutputStream : public OutputStream<char>
    {
    public:
        inline ConsoleOutputStream() = default;
        inline ~ConsoleOutputStream()
        {
            Flush();
        }

    public:
        void Write(const char* data, Usize count) override;
        void Flush() override;

    private:
        void ThreadUnsafeFlush_();

    private:
        static constexpr Usize s_BufferSize = 128;

    private:
        char m_Buffer[s_BufferSize];
        char* m_Pointer = m_Buffer;

        Mutex m_Lock;
    };
}
