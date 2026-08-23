#include "Foundation/Filesystem/Filesystem.h"
#include <Windows.h>

#include "Foundation/Diagnostics/SystemException.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

#undef CreateDirectory
#undef CreateSymbolicLink

#undef DeleteFile

namespace Kitsune::Filesystem
{
    inline static HANDLE GetHandleFromPath(
        const WideString& widePath,
        bool openReparsePoint = true)
    {
        DWORD flags = FILE_FLAG_BACKUP_SEMANTICS;
        if (openReparsePoint)
            flags |= FILE_FLAG_OPEN_REPARSE_POINT;

        return ::CreateFileW(
            widePath.Data(),
            0,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            flags,
            nullptr);
    }

    inline static FILE_ATTRIBUTE_TAG_INFO GetFileAttributeTagInfo(HANDLE handle)
    {
        FILE_ATTRIBUTE_TAG_INFO attributeInfo = {
            .FileAttributes = INVALID_FILE_ATTRIBUTES,
            .ReparseTag = 0
        };

        ::GetFileInformationByHandleEx(
            handle,
            FileAttributeTagInfo,
            &attributeInfo,
            sizeof(attributeInfo));

        return attributeInfo;
    }

    inline static FILE_ATTRIBUTE_TAG_INFO GetFileAttributeTagInfo(
        const WideString& path,
        bool openReparsePoint = true)
    {
        HANDLE handle = GetHandleFromPath(path, openReparsePoint);
        return GetFileAttributeTagInfo(handle);
    }

    bool CreateDirectory(PathView path)
    {
        return ::CreateDirectoryW(path.Native().Raw(), nullptr);
    }

    bool CreateSymbolicLink(PathView path, PathView target)
    {
        WideString wideTarget = target.Native();
        DWORD fileAttribs = ::GetFileAttributesW(wideTarget.Raw());

        if (fileAttribs == INVALID_FILE_ATTRIBUTES)
            return false;

        DWORD flags = SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
        if (fileAttribs & FILE_ATTRIBUTE_DIRECTORY)
            flags |= SYMBOLIC_LINK_FLAG_DIRECTORY;

        return ::CreateSymbolicLinkW(
            path.Native().Raw(),
            wideTarget.Raw(),
            flags);
    }

    bool FileExists(PathView path, bool followSymlinks)
    {
        FILE_ATTRIBUTE_TAG_INFO info = GetFileAttributeTagInfo(
            path.Native(),
            !followSymlinks);

        return (info.FileAttributes != INVALID_FILE_ATTRIBUTES);
    }

    Usize GetFileSize(PathView path, bool followSymlinks)
    {
        HANDLE handle = GetHandleFromPath(path.Native(), !followSymlinks);
        if (handle == INVALID_HANDLE_VALUE)
        {
            throw InvalidArgumentException(
                "The file at the specified path does not exist.");
        }

        FILE_ATTRIBUTE_TAG_INFO info = GetFileAttributeTagInfo(handle);
        if (info.FileAttributes == INVALID_FILE_ATTRIBUTES)
            throw SystemException("Failed to get a file's attributes.");

        if (info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            throw InvalidArgumentException("The specified path points to a directory.");

        LARGE_INTEGER fileSize;
        if (!::GetFileSizeEx(handle, &fileSize))
            throw SystemException("Failed to get the size of a file.");

        return fileSize.QuadPart;
    }

    FileType GetFileType(PathView path, bool followSymlinks)
    {
        FILE_ATTRIBUTE_TAG_INFO info = GetFileAttributeTagInfo(
            path.Native(),
            !followSymlinks);

        if (info.FileAttributes == INVALID_FILE_ATTRIBUTES)
        {
            throw SystemException(
                "Cannot get the information of a file which doesn't exist.");
        }

        if (info.FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            return FileType::Directory;
        else if (info.FileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
        {
            return (info.ReparseTag & IO_REPARSE_TAG_SYMLINK) ?
                FileType::SymbolicLink :
                FileType::File;
        }
        else
        {
            return FileType::File;
        }
    }

}
