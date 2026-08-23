#pragma once

#include "Foundation/Filesystem/PathView.h"

namespace Kitsune::Filesystem
{
    enum class FileType
    {
        File,
        Directory,
        SymbolicLink,
    };

    KITSUNE_API bool CreateDirectory(PathView path);
    KITSUNE_API bool CreateSymbolicLink(PathView path, PathView target);

    KITSUNE_API bool FileExists(PathView path, bool followSymlink = false);

    KITSUNE_API Usize GetFileSize(PathView path, bool followSymlink = false);
    KITSUNE_API FileType GetFileType(PathView path, bool followSymlink = false);

    inline bool IsRegularFile(PathView path, bool followSymlink = false)
    {
        return (GetFileType(path, followSymlink) == FileType::File);
    }

    inline bool IsDirectory(PathView path, bool followSymlink = false)
    {
        return (GetFileType(path, followSymlink) == FileType::Directory);
    }

    inline bool IsSymbolicLink(PathView path, bool followSymlink = false)
    {
        return (GetFileType(path, followSymlink) == FileType::SymbolicLink);
    }
}
