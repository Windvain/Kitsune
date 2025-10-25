#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Logging/IStream.h"

#include "Foundation/Threading/Mutex.h"

namespace Kitsune
{
    class ConsoleOutputStream : public IWriteStream<char>
    {
    public:
        ~ConsoleOutputStream();

        void Write(const char* data, Usize count) override;
        void Flush() override;

    private:
        void WriteToConsole(const NativeChar* begin, const NativeChar* end);
        void ThreadUnsafeFlush();

    private:
        static constexpr Usize s_BufferSize = 128;

    private:
        char m_Buffer[s_BufferSize];
        char* m_Pointer = m_Buffer;

        Mutex m_Lock;
    };
}
