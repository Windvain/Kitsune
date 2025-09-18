#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Logging/IStream.h"

namespace Kitsune
{
    class ConsoleInputStream : public IReadStream<char>
    {
    public:
        inline void Read(IWriteStream<char>& stream) override
        {
            Read(stream, '\n');
        }

        KITSUNE_API_ void Read(IWriteStream<char>& stream, char delim) override;

    private:
        KITSUNE_API_ void ReadFromConsole();

    private:
        static constexpr Usize s_BufferSize = 128;

    private:
        char m_Buffer[s_BufferSize];
        char* m_Pointer = m_Buffer;
    };
}
