#pragma once

#include "Foundation/Streams/Stream.h"
#include "Foundation/Threading/LockGuard.h"

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/String/Valid.h"
#include "Foundation/String/String.h"
#include "Foundation/String/Utf8Encoding.h"

namespace Kitsune
{
    // Specifies the stream which the writer would write to.
    enum class ConsoleWriterType
    {
        StdOut,         //< The console writer outputs data to the stdout stream.
        StdErr          //< The console writer outputs data to the stderr stream.
    };

    namespace Details
    {
        // Can't put this in the BasicConsoleWriter b.c its a templated
        // class.
        KITSUNE_API void UnbufferedWriteConsole(
            ConsoleWriterType streamType,
            StringView string);
    }

    // A `Writer` class which writes UTF-8 data into the console.
    template<Usize BufferSize, Allocator Alloc = GlobalAllocator>
    class BasicConsoleWriter : public Writer<char>
    {
    public:
        static_assert(
            BufferSize >= Utf8Encoding<char>::MaximumCodeunits(),
            "BasicConsoleWriter<BufSize, Alloc> expects the buffer size to "
            "be at least 4 bytes.");

    public:
        inline explicit BasicConsoleWriter(ConsoleWriterType type,
                                           const Alloc& allocator = Alloc())
            : m_Buffer(BufferSize, allocator),
              m_Type(type)
        {
        }

        inline BasicConsoleWriter(BasicConsoleWriter&& consoleWriter)
        {
            LockGuard lockGuard(consoleWriter.m_Lock);

            m_Buffer = Move(consoleWriter.m_Buffer);
            m_Type = consoleWriter.m_Type;
        }

        inline ~BasicConsoleWriter() override
        {
            Flush();
        }

    public:
        inline BasicConsoleWriter& operator=(BasicConsoleWriter&& consoleWriter)
        {
            if (this == &consoleWriter)
                return *this;

            LockGuard lockGuard(m_Lock);
            LockGuard otherLockGuard(consoleWriter.m_Lock);

            m_Buffer = Move(consoleWriter.m_Buffer);
            m_Type = consoleWriter.m_Type;

            return *this;
        }

    public:
        inline void Write(const char* data, Usize dataCount) override
        {
            return Write(StringView(data, dataCount));
        }

        inline void Write(StringView string)
        {
            LockGuard lockGuard(m_Lock);
            while (!string.IsEmpty())
            {
                Usize writeCount = Maths::Minimum(
                    string.Size(),
                    BufferSize - m_Buffer.Size());

                m_Buffer.Append(string.Data(), writeCount);
                string.RemovePrefix(writeCount);

                if (m_Buffer.Size() == BufferSize)
                    ThreadUnsafeFlush();
            }
        }

        inline void Flush() override
        {
            LockGuard lockGuard(m_Lock);
            ThreadUnsafeFlush();
        }

    public:
        [[nodiscard]]
        inline Alloc& GetAllocator()
        {
            return m_Buffer.GetAllocator();
        }

        [[nodiscard]]
        inline const Alloc& GetAllocator() const
        {
            return m_Buffer.GetAllocator();
        }

        inline void Swap(BasicConsoleWriter& consoleWriter)
        {
            LockGuard lockGuard(m_Lock);
            LockGuard otherLockGuard(consoleWriter.m_Lock);

            Kitsune::Swap(m_Buffer, consoleWriter.m_Buffer);
            Kitsune::Swap(m_Type, consoleWriter.m_Type);
        }

    private:
        inline void ThreadUnsafeFlush()
        {
            StringView dataView = m_Buffer;
            while (!dataView.IsEmpty())
            {
                auto invalidIter = FindInvalidEncoding<Utf8Encoding<char>>(
                    dataView.GetBegin(), dataView.GetEnd());

                if (invalidIter != dataView.GetBegin())
                {
                    StringView writeView(dataView.GetBegin(), invalidIter);

                    Details::UnbufferedWriteConsole(m_Type, writeView);
                    dataView.RemovePrefix(writeView.Size());
                }

                if (invalidIter == dataView.GetEnd())
                    continue;

                // If the invalid character appears close to the end of the buffer,
                // it is possible that it got cut off, bring the invalid character
                // back to the front.
                Usize difference = dataView.GetEnd() - invalidIter;
                if (difference < Utf8Encoding<char>::MaximumCodeunits())
                {
                    m_Buffer = String(invalidIter, difference);
                    return;     // Don't clear the buffer, we've cleared it already!
                }
                else
                {
                    Details::UnbufferedWriteConsole(m_Type, "\uFFFD");
                    dataView.RemovePrefix(1);
                }
            }

            m_Buffer.Clear();
        }

    private:
        BasicString<char, Alloc> m_Buffer;
        Mutex m_Lock;

        ConsoleWriterType m_Type;
    };

    using ConsoleWriter = BasicConsoleWriter<4096>;
}
