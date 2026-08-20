#pragma once

#include "Foundation/Streams/FileStream.h"

#include "Foundation/String/UTF8Encoding.h"
#include "Foundation/String/UTF16Encoding.h"
#include "Foundation/String/UTF32Encoding.h"

#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    template<
        TextEncoding Encoding,
        Usize BufSize,
        Allocator Alloc = GlobalAllocator>
    class BasicFileWriter
    {
    public:
        using ValueType = typename Encoding::CodeunitType;
        using StreamType = BasicFileStream<BufSize, Alloc>;

        using EncodingType = Encoding;

    public:
        inline BasicFileWriter(const Alloc& allocator = Alloc())
            : m_Stream(allocator)
        {
        }

        inline BasicFileWriter(StreamType&& stream)
            : m_Stream(Move(stream))
        {
            if (m_Stream.IsOpen() && !m_Stream.IsWritable())
                throw InvalidArgumentException("The stream is not writable!");
        }

        inline BasicFileWriter(Filesystem::PathView filePath,
                               FileOpenMode openMode)
            : BasicFileWriter(filePath, FileAccessMode::Write, openMode)
        {
        }

        inline BasicFileWriter(Filesystem::PathView filePath,
                               FileAccessMode accessMode = FileAccessMode::Write,
                               FileOpenMode openMode = FileOpenMode::Open)
        {
            if (!Open(filePath, accessMode, openMode))
                throw SystemException("Failed to open a stream to the file.");
        }

        inline BasicFileWriter(BasicFileWriter&&) = default;
        inline BasicFileWriter& operator=(BasicFileWriter&&) = default;

    public:
        [[nodiscard]]
        inline const Filesystem::Path& GetPath() const
        {
            return m_Stream.GetPath();
        }

        [[nodiscard]]
        inline const StreamType& GetStream() const
        {
            return m_Stream;
        }

    public:
        [[nodiscard]]
        inline Alloc& GetAllocator()
        {
            return m_Stream.GetAllocator();
        }

        [[nodiscard]]
        inline const Alloc& GetAllocator() const
        {
            return m_Stream.GetAllocator();
        }

    public:
        inline bool Open(Filesystem::PathView filePath,
                         FileOpenMode openMode)
        {
            return Open(filePath, FileAccessMode::Write, openMode);
        }

        inline bool Open(Filesystem::PathView filePath,
                         FileAccessMode accessMode = FileAccessMode::Write,
                         FileOpenMode openMode = FileOpenMode::Open)
        {
            if (accessMode == FileAccessMode::Read)
            {
                throw InvalidArgumentException(
                    "Failed to create a FileWriter. The `accessMode` argument should "
                    "not be set to `FileAccessMode::Read`.");
            }

            return m_Stream.Open(filePath, accessMode, openMode);
        }

        inline void Close()
        {
            m_Stream.Close();
        }

        [[nodiscard]]
        inline bool IsOpen() const
        {
            return m_Stream.IsOpen();
        }

    public:
        inline void Write(const ValueType* data, Usize count)
        {
            if (m_Stream.GetPosition() == 0)
            {
                auto preamble = Encoding::GetPreamble();
                m_Stream.Write(
                    reinterpret_cast<const Byte*>(preamble.Data()),
                    preamble.Size() * sizeof(ValueType));
            }

            m_Stream.Write(
                reinterpret_cast<const Byte*>(data),
                count * sizeof(ValueType));
        }

        inline void Write(BasicStringView<ValueType> string)
        {
            Write(string.Data(), string.Size());
        }

        inline void Flush()
        {
            m_Stream.Flush();
        }

    private:
        BasicFileStream<BufSize, Alloc> m_Stream;
    };

    using FileWriter = BasicFileWriter<UTF8Encoding<char>, 4096>;

    using U8FileWriter = BasicFileWriter<UTF8Encoding<char8_t>, 4096>;
    using U16FileWriter = BasicFileWriter<UTF16Encoding<char16_t>, 4096>;
    using U32FileWriter = BasicFileWriter<UTF32Encoding<char32_t>, 4096>;
}
