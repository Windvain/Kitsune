#include "Foundation/Filesystem/WindowsPath.h"

// Can be included in other implementations to store paths
// meant for only Windows.
#if defined(KITSUNE_OS_WINDOWS)
    #include <Windows.h>
#else
    // Emulate definitions from <Windows.h> header.
    #define DWORD Uint16
    #define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
    #define FILE_ATTRIBUTES_DIRECTORY ((DWORD)0x00000010)
#endif

namespace Kitsune
{
    KITSUNE_FORCEINLINE bool IsSeperator(wchar_t ch)
    {
        return ((ch == '\\') || (ch == '/'));
    }

    KITSUNE_FORCEINLINE DWORD PortableGetFileAttributes(const wchar_t* filepath)
    {
    #if defined(KITSUNE_OS_WINDOWS)
        DWORD attributes = ::GetFileAttributesW(filepath);
        return attributes;
    #else
        return INVALID_FILE_ATTRIBUTES;
    #endif
    }

    bool WindowsPath::Exists() const
    {
        DWORD attributes = PortableGetFileAttributes(m_String.Raw());
        return (attributes != INVALID_FILE_ATTRIBUTES);
    }

    bool WindowsPath::IsFile() const
    {
        DWORD attributes = PortableGetFileAttributes(m_String.Raw());
        return ((attributes != INVALID_FILE_ATTRIBUTES) &&
               !(attributes &  FILE_ATTRIBUTE_DIRECTORY));
    }

    bool WindowsPath::IsDirectory() const
    {
        DWORD attributes = PortableGetFileAttributes(m_String.Raw());
        return ((attributes != INVALID_FILE_ATTRIBUTES) &&
                (attributes &  FILE_ATTRIBUTE_DIRECTORY));
    }

    bool WindowsPath::IsAbsolute() const
    {
        if (m_String.IsEmpty()) return false;

        WideString maybeRoot = m_String.Substring(0,
            KITSUNE_MIN(m_String.Size(), 4));

        if (m_String[1] == L':')
        {
            return (m_String.Size() > 2) ? IsSeperator(m_String[2]) : true;
        }

        if ((m_String[0] == Seperator) && (m_String[1] == Seperator))
            return true;

        if (m_String.Size() >= 4)
        {
            if ((maybeRoot == L"\\\\?\\") || (maybeRoot == L"\\\\.\\"))
                return true;
        }

        return false;
    }
}
