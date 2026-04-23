#include "Foundation/Filesystem/FileStream.h"

#include <Windows.h>
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune::Details
{
    static DWORD GetDesiredAccess(FileAccessMode accessMode)
    {
        switch (accessMode)
        {
        case FileAccessMode::Read:
            return GENERIC_READ;
        case FileAccessMode::Write:
            return GENERIC_WRITE;
        case FileAccessMode::ReadWrite:
            return GENERIC_READ | GENERIC_WRITE;
        }

        KITSUNE_UNREACHABLE();
    }

    static DWORD GetCreationMode(FileOpenMode openMode)
    {
        switch (openMode)
        {
        case FileOpenMode::Append: [[fallthrough]];
        case FileOpenMode::Open:
            return OPEN_EXISTING;

        case FileOpenMode::OpenOrCreate:
            return OPEN_ALWAYS;
        case FileOpenMode::CreateNew:
            return CREATE_ALWAYS;
        case FileOpenMode::Truncate:
            return TRUNCATE_EXISTING;
        }

        KITSUNE_UNREACHABLE();
    }

    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
    // m_OpenMode and m_AccessMode are not initialized if the constructor
    // fails to open the file.
    FileObject::FileObject()
    {
        static_assert(
            s_BufferSize >= sizeof(HANDLE),
            "Cannot store the Windows file handle in s_BufferSize bytes.");

        *reinterpret_cast<HANDLE*>(m_Buffer) = INVALID_HANDLE_VALUE;
    }

    FileObject::FileObject(FileObject&& fileObject)
        : m_OpenMode(fileObject.m_OpenMode),
          m_AccessMode(fileObject.m_AccessMode)
    {
        std::memcpy(m_Buffer, fileObject.m_Buffer, s_BufferSize);
        *reinterpret_cast<HANDLE*>(fileObject.m_Buffer) = INVALID_HANDLE_VALUE;
    }
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)

    FileObject& FileObject::operator=(FileObject&& fileObject)
    {
        if (this == &fileObject)
            return *this;

        if (IsOpen())
            Close();

        std::memcpy(m_Buffer, fileObject.m_Buffer, s_BufferSize);
        *reinterpret_cast<HANDLE*>(fileObject.m_Buffer) = INVALID_HANDLE_VALUE;

        m_OpenMode = fileObject.m_OpenMode;
        m_AccessMode = fileObject.m_AccessMode;

        return *this;
    }

    void FileObject::Write(const Byte* data, Usize dataCount)
    {
        KITSUNE_ASSERT(IsWritable(), "The file should be writable.");

        HANDLE handle = *reinterpret_cast<HANDLE*>(m_Buffer);
        DWORD writtenCount_;

        if (m_OpenMode == FileOpenMode::Append)
            Seek(0, SeekOrigin::End);

        if (!::WriteFile(handle, data, static_cast<DWORD>(dataCount), &writtenCount_,
                         nullptr))
        {
            throw SystemException("Failed to write to the underlying file handle.");
        }
    }

    Usize FileObject::Read(Byte* buffer, Usize bufferSize)
    {
        KITSUNE_ASSERT(IsReadable(), "The file should be readable.");

        HANDLE handle = *reinterpret_cast<HANDLE*>(m_Buffer);
        DWORD readCount;

        if (!::ReadFile(handle, buffer, static_cast<DWORD>(bufferSize), &readCount,
                        nullptr))
        {
            throw SystemException("Failed to read from the underlying file handle.");
        }

        return readCount;
    }

    Usize FileObject::Seek(Ptrdiff offset, SeekOrigin origin)
    {
        KITSUNE_ASSERT(IsSeekable(), "The file should be seekable.");

        LARGE_INTEGER filePointer;
        LARGE_INTEGER zero = { .QuadPart = offset };

        DWORD winOrigin = FILE_CURRENT;     // Shut MSVC up.
        switch (origin)
        {
        case SeekOrigin::Begin:
            winOrigin = FILE_BEGIN;
            break;
        case SeekOrigin::Current:
            winOrigin = FILE_CURRENT;
            break;
        case SeekOrigin::End:
            winOrigin = FILE_END;
            break;
        }

        HANDLE handle = *reinterpret_cast<HANDLE*>(m_Buffer);
        if (!::SetFilePointerEx(handle, zero, &filePointer, winOrigin))
            throw SystemException("Failed to set the file pointer.");

        return filePointer.QuadPart;
    }

    bool FileObject::IsOpen() const
    {
        HANDLE handle = *reinterpret_cast<const HANDLE*>(m_Buffer);
        return (handle != INVALID_HANDLE_VALUE);
    }

    bool FileObject::IsReadable() const
    {
        KITSUNE_ASSERT(IsOpen(), "A file had not been opened.");
        return (m_AccessMode != FileAccessMode::Write);
    }

    bool FileObject::IsWritable() const
    {
        KITSUNE_ASSERT(IsOpen(), "A file had not been opened.");
        return (m_AccessMode != FileAccessMode::Read);
    }

    bool FileObject::IsSeekable() const
    {
        KITSUNE_ASSERT(IsOpen(), "A file had not been opened.");
        return (m_OpenMode != FileOpenMode::Append);
    }

    Usize FileObject::GetPosition() const
    {
        KITSUNE_ASSERT(IsOpen(), "A file had not been opened.");

        LARGE_INTEGER position;
        LARGE_INTEGER zero = { /* ... */ };

        HANDLE handle = *reinterpret_cast<const HANDLE*>(m_Buffer);
        if (!::SetFilePointerEx(handle, zero, &position, FILE_CURRENT))
            throw SystemException("Failed to retrieve the value of the file pointer.");

        return position.QuadPart;
    }

    bool FileObject::Open(StringView filePath,
                          FileAccessMode accessMode,
                          FileOpenMode openMode)
    {
        if (IsOpen())
            return false;

        // Sanitize file access mode.
        {
            bool assumeWritable =
                (openMode == FileOpenMode::Append) ||
                (openMode == FileOpenMode::Truncate);

            if (assumeWritable && (accessMode == FileAccessMode::Read))
                accessMode = FileAccessMode::ReadWrite;
        }

        DWORD desiredAccess = GetDesiredAccess(accessMode);
        DWORD creationMode = GetCreationMode(openMode);

        WideString wideFilePath = Utf8ToUtf16<char, wchar_t>(filePath);

        auto* handleStore = reinterpret_cast<HANDLE*>(m_Buffer);
        *handleStore = ::CreateFileW(
            wideFilePath.Raw(),
            desiredAccess,
            0, nullptr,
            creationMode,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

        if (*handleStore == INVALID_HANDLE_VALUE)
            return false;

        if (openMode == FileOpenMode::Append)
            Seek(0, SeekOrigin::End);

        m_OpenMode = openMode;
        m_AccessMode = accessMode;

        return true;
    }

    void FileObject::Close()
    {
        if (!IsOpen())
            return;

        auto* handleStore = reinterpret_cast<HANDLE*>(m_Buffer);
        KITSUNE_VERIFY(
            ::CloseHandle(*handleStore),
            "Failed to close the file handle.");

        *handleStore = INVALID_HANDLE_VALUE;
    }
}
