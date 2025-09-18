#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Logging/IStream.h"

#include "Foundation/Threading/Mutex.h"

namespace Kitsune
{
    class ConsoleOutputStream : public IWriteStream<char>
    {
    public:
        KITSUNE_API_ ~ConsoleOutputStream();

        KITSUNE_API_ void Write(const char* data, Usize count) override;
        KITSUNE_API_ void Flush() override;

    private:
        KITSUNE_API_ void WriteToConsole(const NativeChar* begin,
                                         const NativeChar* end);

        KITSUNE_API_ void ThreadUnsafeFlush();

    private:
        static constexpr Usize s_BufferSize = 128;

    private:
        char m_Buffer[s_BufferSize];
        char* m_Pointer = m_Buffer;

        Mutex m_Lock;
    };
}
