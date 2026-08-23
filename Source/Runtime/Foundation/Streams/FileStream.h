#pragma once

#include "Foundation/Streams/Stream.h"

#include "Foundation/Filesystem/Path.h"
#include "Foundation/Templates/Exchange.h"

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/LogicException.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    // The access requested when opening a file.
    enum class FileAccessMode
    {
        Read,       //< Requested that a file can be read from.
        Write,      //< Requested that a file can be written to.
        ReadWrite   //< Requested that a file can both be read from and
                    // written to.
    };

    // The mode at which a file is opened.
    enum class FileOpenMode
    {
        Append,         //< The file is opened with the write pointer locked to
                        //  the end of the file.
        Truncate,       //< Opens a file and removes all of its contents.
        CreateNew,      //< Creates a file if and only if it doesn't exist.
        Open,           //< Opens a file if and only if it exists.
        OpenOrCreate    //< Opens a file if it exists, and creates a file
                        //  if it doesn't.
    };

    namespace Details
    {
        // Contains platform-specific implementations for file operations.
        // Kind of like the FILE struct in C.
        class KITSUNE_API FileObject
        {
        public:
            FileObject();

            FileObject(FileObject&& fileObject);
            FileObject& operator=(FileObject&& fileObject);

        public:
            bool Open(Filesystem::PathView filePath,
                      FileAccessMode accessMode,
                      FileOpenMode openMode = FileOpenMode::Open);

            void Close();

        public:
            void Write(const Byte* data, Usize count);
            Usize Read(Byte* buffer, Usize bufferSize);

            Usize Seek(Ptrdiff offset, SeekOrigin origin);
            [[nodiscard]] Usize Size() const;

        public:
            [[nodiscard]] bool IsOpen() const;

            [[nodiscard]] bool IsReadable() const;
            [[nodiscard]] bool IsWritable() const;
            [[nodiscard]] bool IsSeekable() const;

            [[nodiscard]]
            Usize GetPosition() const;

            [[nodiscard]]
            inline Filesystem::PathView GetName() const
            {
                KITSUNE_ASSERT(IsOpen(), "Failed to get the path of this stream.");
                return m_Name;
            }

        private:
            static constexpr Usize s_BufferSize = 16;

        private:
            // File handles/descriptors in most operating systems are pretty
            // small, so there is no need to allocate them on the heap.
            Byte m_Buffer[s_BufferSize];

            FileOpenMode m_OpenMode;
            FileAccessMode m_AccessMode;

            Filesystem::Path m_Name;
        };
    }

    // Provides platform-independent access to a file.
    template<Usize BufferSize, Allocator Alloc = GlobalAllocator>
    class BasicFileStream
    {
    public:
        inline BasicFileStream(const Alloc& allocator = Alloc())
            : m_Allocator(allocator)
        {
        }

        inline BasicFileStream(Filesystem::PathView filePath,
                               FileAccessMode accessMode,
                               FileOpenMode openMode = FileOpenMode::Open)
            : BasicFileStream()
        {
            if (!Open(filePath, accessMode, openMode))
                throw SystemException("Failed to open the specified file.");
        }

        inline BasicFileStream(const BasicFileStream&) = delete;

        inline BasicFileStream(BasicFileStream&& fileStream)
            : m_FileObject(Move(fileStream.m_FileObject)),
              m_Buffer(Exchange(fileStream.m_Buffer, nullptr)),
              m_ReadPosition(Exchange(fileStream.m_ReadPosition, nullptr)),
              m_WritePosition(Exchange(fileStream.m_WritePosition, nullptr)),
              m_SeekPosition(Exchange(fileStream.m_SeekPosition, nullptr)),
              m_Allocator(Move(fileStream.m_Allocator))
        {
        }

        inline ~BasicFileStream()
        {
            if (IsOpen())
                Close();
        }

    public:
        inline BasicFileStream& operator=(const BasicFileStream&) = delete;

        inline BasicFileStream& operator=(BasicFileStream&& fileStream)
        {
            if (this == &fileStream)
                return *this;

            BasicFileStream(Move(fileStream)).Swap(*this);
            return *this;
        }

    public:
        inline bool Open(Filesystem::PathView filePath,
                         FileAccessMode accessMode,
                         FileOpenMode openMode = FileOpenMode::Open)
        {
            if (IsOpen())
                return false;

            bool success = m_FileObject.Open(filePath, accessMode, openMode);
            if (!success)
                return success;

            KITSUNE_ASSERT(m_Buffer == nullptr, "m_Buffer should not have been set.");

            m_Buffer = static_cast<Byte*>(m_Allocator.Allocate(BufferSize));
            m_ReadPosition = m_WritePosition = m_SeekPosition =
                m_Buffer;

            return success;
        }

        inline void Close()
        {
            if (!IsOpen())
                return;

            // Flush all output data before closing the file stream.
            if (IsWritable())
                Flush();

            KITSUNE_ASSERT(m_Buffer != nullptr, "m_Buffer should have been allocated.");

            m_Allocator.Free(m_Buffer, BufferSize);
            m_Buffer = nullptr;

            m_ReadPosition = m_WritePosition = m_SeekPosition = m_Buffer;

            m_FileObject.Close();
        }

        [[nodiscard]]
        inline bool IsOpen() const
        {
            return m_FileObject.IsOpen();
        }

    public:
        inline void Write(const Byte* data, Usize dataCount)
        {
            if (!IsWritable())
            {
                throw LogicException(
                    "The stream does not have write permissions to "
                    "the opened file.");
            }

            if (m_ReadPosition != m_Buffer)
            {
                // The buffer still contains read data, sync the seek pointer and
                // reset the buffer.
                Ptrdiff offset = m_SeekPosition - m_ReadPosition;
                Seek(offset, SeekOrigin::Current);

                m_ReadPosition = m_SeekPosition = m_Buffer;
            }

            // Redirect large writes directly to the underlying stream, buffers
            // slow them down instead of optimizing them.
            if (dataCount >= BufferSize)
            {
                Flush();
                m_FileObject.Write(data, dataCount);

                return;
            }

            const Byte* bufferEnd = m_Buffer + BufferSize;
            const Byte* dataEnd = data + dataCount;

            while (data != dataEnd)
            {
                Usize writeCount = Maths::Minimum(
                    static_cast<Usize>(dataEnd - data),
                    static_cast<Usize>(bufferEnd - m_WritePosition));

                std::memcpy(
                    m_WritePosition,
                    data,
                    writeCount * sizeof(Byte));

                m_WritePosition += writeCount;
                data += writeCount;

                if (m_WritePosition == bufferEnd)
                    Flush();
            }
        }

        inline Usize Read(Byte* buffer, Usize bufferSize)
        {
            if (!IsReadable())
            {
                throw LogicException(
                    "The stream does not have read permissions to "
                    "the opened file.");
            }

            if (m_WritePosition != m_Buffer)
                Flush();

            Usize writtenCount = 0;
            while (bufferSize != 0)
            {
                if (m_SeekPosition == m_ReadPosition)
                {
                    Usize fillCount = m_FileObject.Read(m_Buffer, BufferSize);
                    if (fillCount == 0)
                        break;

                    m_SeekPosition = m_Buffer;
                    m_ReadPosition = m_Buffer + fillCount;
                }

                Usize offset = Maths::Minimum(
                    Usize(m_ReadPosition - m_SeekPosition),
                    bufferSize);

                std::memcpy(buffer, m_SeekPosition, offset * sizeof(Byte));

                m_SeekPosition += offset;
                writtenCount += offset;

                bufferSize -= offset;
                buffer += offset;
            }

            return writtenCount;
        }

        inline Usize Seek(Ptrdiff offset, SeekOrigin origin)
        {
            if (!IsSeekable())
            {
                throw LogicException(
                    "The file stream is not seekable. If you had called Open() "
                    "with FileOpenMode::Append, the file stream will not be seekable.");
            }

            // Flush anything we were writing before we move the file pointer.
            if (IsWritable())
                Flush();

            Byte* newPosition = m_SeekPosition + offset;
            if ((origin != SeekOrigin::Current) || (newPosition < m_Buffer) ||
                (newPosition >= m_ReadPosition))
            {
                m_FileObject.Seek(offset, origin);
                m_ReadPosition = m_SeekPosition = m_Buffer;
            }
            else
            {
                // We are in the range already read by the stream, just set
                // the pointer!
                m_SeekPosition = newPosition;
            }

            return GetPosition();
        }

        [[nodiscard]]
        inline Usize Length() const
        {
            if (!IsOpen())
            {
                throw LogicException(
                    "Could not get the length of a file stream which does not hold "
                    "a file handle/descriptor.");
            }

            return m_FileObject.Size();
        }

    public:
        [[nodiscard]]
        inline bool IsReadable() const
        {
            if (!IsOpen())
            {
                throw LogicException(
                    "Cannot get the properties of a stream without an opened "
                    "file. Did you forget to call Open()?");
            }

            return m_FileObject.IsReadable();
        }

        [[nodiscard]]
        inline bool IsWritable() const
        {
            if (!IsOpen())
            {
                throw LogicException(
                    "Cannot get the properties of a stream without an opened "
                    "file. Did you forget to call Open()?");
            }

            return m_FileObject.IsWritable();
        }

        [[nodiscard]]
        inline bool IsSeekable() const
        {
            if (!IsOpen())
            {
                throw LogicException(
                    "Cannot get the properties of a stream without an opened "
                    "file. Did you forget to call Open()?");
            }

            return m_FileObject.IsSeekable();
        }

    public:
        [[nodiscard]]
        inline Alloc& GetAllocator()
        {
            return m_Allocator;
        }

        [[nodiscard]]
        inline const Alloc& GetAllocator() const
        {
            return m_Allocator;
        }

    public:
        inline void Flush()
        {
            if (!IsWritable())
                throw LogicException("Cannot flush a read stream.");

            m_FileObject.Write(m_Buffer, m_WritePosition - m_Buffer);
            m_WritePosition = m_Buffer;
        }

        [[nodiscard]]
        inline Usize GetPosition() const
        {
            if (!IsOpen())
            {
                throw LogicException(
                    "Cannot get the properties of a stream without an opened "
                    "file. Did you forget to call Open()?");
            }

            Usize pos = m_FileObject.GetPosition();
            if (m_WritePosition != m_Buffer)
                return pos + (m_WritePosition - m_Buffer);

            return pos - (m_ReadPosition - m_SeekPosition);
        }

        [[nodiscard]]
        inline Filesystem::PathView GetPath() const
        {
            return m_FileObject.GetName();
        }

        inline void Swap(BasicFileStream& stream)
        {
            Kitsune::Swap(m_FileObject, stream.m_FileObject);
            Kitsune::Swap(m_Buffer, stream.m_Buffer);

            Kitsune::Swap(m_ReadPosition, stream.m_ReadPosition);
            Kitsune::Swap(m_WritePosition, stream.m_WritePosition);
            Kitsune::Swap(m_SeekPosition, stream.m_SeekPosition);

            Kitsune::Swap(m_Allocator, stream.m_Allocator);
        }

    private:
        Details::FileObject m_FileObject;
        Byte* m_Buffer = nullptr;

        Byte* m_ReadPosition = nullptr;
        Byte* m_WritePosition = nullptr;
        Byte* m_SeekPosition = nullptr;

        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };

    using FileStream = BasicFileStream<4096, GlobalAllocator>;
}
