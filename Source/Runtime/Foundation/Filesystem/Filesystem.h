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

    [[nodiscard]] KITSUNE_API bool CreateDirectory(PathView path);
    [[nodiscard]] KITSUNE_API bool CreateSymbolicLink(PathView path, PathView target);

    [[nodiscard]]
    KITSUNE_API bool FileExists(PathView path, bool followSymlink = false);

    [[nodiscard]]
    KITSUNE_API Usize GetFileSize(PathView path, bool followSymlink = false);

    [[nodiscard]]
    KITSUNE_API FileType GetFileType(PathView path, bool followSymlink = false);

    [[nodiscard]]
    inline bool IsRegularFile(PathView path, bool followSymlink = false)
    {
        return (GetFileType(path, followSymlink) == FileType::File);
    }

    [[nodiscard]]
    inline bool IsDirectory(PathView path, bool followSymlink = false)
    {
        return (GetFileType(path, followSymlink) == FileType::Directory);
    }

    [[nodiscard]]
    inline bool IsSymbolicLink(PathView path, bool followSymlink = false)
    {
        return (GetFileType(path, followSymlink) == FileType::SymbolicLink);
    }
}
