#include "Foundation/Filesystem/WindowsPath.h"
#include "Foundation/Algorithms/Find.h"

#if defined(KITSUNE_OS_WINDOWS)
    #define KITSUNE_UTF16_PATH_(str) L ## str
#else
    #define KITSUNE_UTF16_PATH_(str) u ## str
#endif

namespace Kitsune::Filesystem
{
    KITSUNE_FORCEINLINE bool IsWindowsSeperator(wchar_t ch)
    {
        return ((ch == KITSUNE_UTF16_PATH_('\\')) || (ch == KITSUNE_UTF16_PATH_('/')));
    }

    WindowsPath WindowsPath::GetDrivePath() const
    {
        return GetDriveSubstring();
    }

    WindowsPath WindowsPath::GetRootPath() const
    {
        return GetRootSubstring();
    }

    WindowsPath WindowsPath::GetRelativePath() const
    {
        return GetRelativeSubstring();
    }

    WindowsPath WindowsPath::GetParentPath() const
    {
        ViewType relPath = GetRelativeSubstring();
        if (relPath.IsEmpty()) return *this;

        auto it = relPath.GetEnd() - 1;
        for (; it != relPath.GetBegin(); --it)
        {
            if (IsWindowsSeperator(*it) && !IsWindowsSeperator(*(it - 1)))
                break;
        }

        relPath.RemoveSuffix(relPath.GetEnd() - it);
        return GetAnchorPath() + relPath;
    }

    WindowsPath::ViewType WindowsPath::GetDriveSubstring() const
    {
        // Source: https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats

        if (m_String.IsEmpty()) return ViewType(m_String.Raw());
        WideString maybeRoot = m_String.Substring(0, KITSUNE_MIN(m_String.Size(), 3));

        // Regular roots on Windows are composed of the volume name (C) and the suffix (:).
        if (maybeRoot.Size() < 2)
            return ViewType(m_String.Raw(), 0);

        if (maybeRoot[1] == KITSUNE_UTF16_PATH_(':'))
            return ViewType(m_String.Raw(), 2);

        // Then check for UNC and DOS paths.
        if (IsWindowsSeperator(maybeRoot[0]) && IsWindowsSeperator(maybeRoot[1]))
        {
            auto it = Algorithms::FindIf(m_String.GetBegin() + 2, m_String.GetEnd(), IsWindowsSeperator);
            return ViewType(m_String.Raw(), it - m_String.GetBegin());
        }

        return ViewType(m_String.Raw(), 0);
    }

    WindowsPath::ViewType WindowsPath::GetRootSubstring() const
    {
        ViewType drivePath = GetDriveSubstring();
        if (drivePath.Size() == m_String.Size())
            return ViewType(drivePath.GetEnd(), 0);

        CharType rootPath = m_String[drivePath.Size()];
        return IsWindowsSeperator(rootPath) ? ViewType(m_String.GetBegin() + drivePath.Size(), 1) :
                                              ViewType(drivePath.GetEnd(), 0);
    }

    WindowsPath::ViewType WindowsPath::GetRelativeSubstring() const
    {
        ViewType root = GetRootSubstring();
        return ViewType(root.GetEnd(), m_String.GetEnd());
    }
}
