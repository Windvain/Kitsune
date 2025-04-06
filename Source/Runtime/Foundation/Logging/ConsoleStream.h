#pragma once

#include "Foundation/Common/Macros.h"

#include "Foundation/Logging/IStream.h"
#include "Foundation/Logging/StreamBuffer.h"

namespace Kitsune
{
    class ConsoleOutputStream : public IWriteStream<char>
    {
    public:
        inline explicit ConsoleOutputStream(bool useStderr)
            : m_ErrorStream(useStderr), m_RawBuffer(),
              m_Buffer(m_RawBuffer, m_RawBuffer + BufferSize)
        {
        }

    public:
        KITSUNE_API_ void Write(const char* ptr, Usize count) override;
        inline void Flush() override { Overflow(); }

    private:
        KITSUNE_API_ void Overflow();

    private:
        static constexpr Usize BufferSize = 128;

    private:
        bool m_ErrorStream;

        char m_RawBuffer[BufferSize];
        StreamBuffer<char> m_Buffer;
    };

    class ConsoleInputStream : public IReadStream<char>
    {
    public:
        ConsoleInputStream(IWriteStream<char>& stream)
            : m_OutStream(stream), m_RawBuffer(),
              m_Buffer(m_RawBuffer, m_RawBuffer + BufferSize)
        {
        }

    public:
        KITSUNE_API_ void Read(Usize count) override;
        KITSUNE_API_ void Read() override;

    private:
        KITSUNE_API_ void Underflow();

    private:
        static constexpr Usize BufferSize = 128;

    private:
        IWriteStream<char>& m_OutStream;

        char m_RawBuffer[BufferSize];
        StreamBuffer<char> m_Buffer;
    };
}
