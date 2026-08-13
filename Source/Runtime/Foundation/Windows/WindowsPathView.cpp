#include "Foundation/Filesystem/PathView.h"

namespace Kitsune
{
    namespace
    {
        const StringView RootPrefixes[3] = {
            R"(\\.\)",
            R"(\\?\)",
            R"(\\)"      // Keep this after the device paths because they conflict.
        };
    }

    inline static bool IsInvalidPathChar(char ch)
    {
        // Any of these characters & characters whose integer values are < 32 is invalid.
        return ((ch == '<') || (ch == '>') || (ch == ':') || (ch == '\"') ||
                (ch == '|') || (ch == '?') || (ch == '*') || (ch < 32));
    }

    inline static bool IsDirectorySeperator(char ch)
    {
        return ((ch == '\\') || (ch == '/'));
    }

    bool PathView::IsValidPath() const
    {
        // Just check for invalid characters, don't need to check for reserved filenames,
        // i.e. NUL.txt, LPT1-9, etc. These filenames are only partially documented,
        // so some names might fall under the radar. For example, LPT² is invalid, but
        // LPT⁴ is fine.
        StringView relativePath = GetRelativePath();
        auto invalid = Algorithms::FindIf(
            relativePath.GetBegin(),
            relativePath.GetEnd(),
            IsInvalidPathChar);

        return (invalid == m_Path.GetEnd());
    }

    bool PathView::IsAbsolute() const
    {
        return !GetRootName().IsEmpty() && !GetRootDirectory().IsEmpty();
    }

    bool PathView::IsRelative() const
    {
        return !IsAbsolute();
    }

    PathView PathView::GetRootName() const
    {
        // We will be returning StringView(m_Path.GetBegin(), 0) as an empty path.
        // This is to make sure that retrieving the root directory works.
        if (m_Path.Size() < 2)
            return StringView(m_Path.GetBegin(), 0);

        // We will assume that all DOS paths' root name will have the format
        // "volume letter + volume seperator", i.e. "C:", "X:".
        if (m_Path[1] == ':')
        {
            // Uppercase and make sure that the drive letter is between 'A' and 'Z'.
            char driveLetter = static_cast<char>(m_Path[0] - 7 * (m_Path[0] > 'Z'));
            return ((driveLetter >= 'A') && (driveLetter <= 'Z')) ?
                StringView(m_Path.Substring(0, 2)) :
                StringView(m_Path.GetBegin(), 0);
        }

        // Device paths are prefixed with \\.\ or \\?\. We assume that the prefix will be
        // in the format "prefix + device name", for example "\\.\C:\" or
        // "\\?\Volume{b75e2c83-0000-0000-0000-602f00000000}".
        // UNC paths are prefixed with \\, abd follow the same rules as the DOS device
        // paths. These backslashes are NOT REPLACEABLE with forward slashes.
        for (const StringView& prefix : RootPrefixes)
        {
            if (!m_Path.StartsWith(prefix))
                continue;

            const auto predicate = [](char ch) -> bool
            {
                return (IsDirectorySeperator(ch) || (ch == ':'));
            };

            auto iter = Algorithms::FindIf(
                m_Path.GetBegin() + prefix.Size(),
                m_Path.GetEnd(),
                predicate);

            if ((iter != m_Path.GetEnd()) && (*iter == ':'))
                ++iter;

            return StringView(m_Path.GetBegin(), iter);
        }

        return StringView(m_Path.GetBegin(), 0);
    }

    PathView PathView::GetRootDirectory() const
    {
        StringView rootName = GetRootName();
        if (rootName.Size() >= m_Path.Size())
            return StringView(rootName.GetEnd(), 0);

        // rootName.GetEnd() should be safely deferenceable.
        return IsDirectorySeperator(*rootName.GetEnd()) ?
            StringView(ToAddress(rootName.GetEnd()), 1) :
            StringView(rootName.GetEnd(), 0);
    }

    PathView PathView::GetRootPath() const
    {
        StringView rootName = GetRootName();
        StringView rootDirectory = GetRootDirectory();

        return PathView(rootName.GetBegin(), rootDirectory.GetEnd());
    }

    PathView PathView::GetRelativePath() const
    {
        StringView rootDirectory = GetRootDirectory();
        return PathView(rootDirectory.GetEnd(), m_Path.GetEnd());
    }

    PathView PathView::GetParentPath() const
    {
        StringView relativePath = GetRelativePath();
        if (relativePath.IsEmpty())
            return *this;

        auto iter = Algorithms::FindLastIf(
            relativePath.GetBegin(), relativePath.GetEnd(), IsDirectorySeperator);

        if (iter == relativePath.GetEnd())
            iter = relativePath.GetBegin();

        StringView parentPath = m_Path;
        parentPath.RemoveSuffix(relativePath.GetEnd() - iter);

        return parentPath;
    }

    PathView PathView::GetFilename() const
    {
        StringView relativePath = GetRelativePath();
        auto iter = Algorithms::FindLastIf(
            relativePath.GetBegin(), relativePath.GetEnd(), IsDirectorySeperator);

        // No directory seperator found, return the entire relative path.
        if (iter == relativePath.GetEnd())
            return relativePath;

        return PathView(++iter, relativePath.GetEnd());
    }

    PathView PathView::GetFileStem() const
    {
        StringView filename = GetFilename();
        auto iter = Algorithms::FindLast(filename.GetBegin(), filename.GetEnd(), '.');

        if (iter == filename.GetBegin())
            return filename;

        return PathView(filename.GetBegin(), iter);
    }

    PathView PathView::GetExtension() const
    {
        StringView filename = GetFilename();
        auto iter = Algorithms::FindLast(filename.GetBegin(), filename.GetEnd(), '.');

        if (iter == filename.GetBegin())
            return PathView();

        return PathView(iter, filename.GetEnd());
    }
}

