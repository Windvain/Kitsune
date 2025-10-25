#pragma once

#include "Foundation/String/String.h"

namespace Kitsune::Filesystem
{
    class WindowsPath
    {
    public:
        // On Windows, we favour the use of wchar_t as it is easier to interop with
        // Win32 API code, while on other platforms, we just require that the character type
        // has a size of 16 bits.
#if defined(KITSUNE_OS_WINDOWS)
        using CharType = wchar_t;

        using StringType = WideString;
        using ViewType = WideStringView;
#else
        using CharType = char16_t;

        using StringType = U16String;
        using ViewType = U16StringView;
#endif

    public:
#if defined(KITSUNE_OS_WINDOWS)
        static constexpr wchar_t PreferredSeperator = L'\\';
#else
        static constexpr char16_t PreferredSeperator = u'\\';
#endif

    public:
        inline WindowsPath() = default;

        inline WindowsPath(const WindowsPath& path) = default;
        inline WindowsPath(WindowsPath&& path) = default;

        inline WindowsPath(StringType&& source)    : m_String(Move(source)) { /* ... */ }
        inline WindowsPath(const CharType* source) : m_String(source)       { /* ... */ }
        inline WindowsPath(const ViewType source)  : m_String(source)       { /* ... */ }

        template<ForwardIterator It>
        inline WindowsPath(It begin, It end)
            : m_String(begin, end)
        {
        }

        inline ~WindowsPath() = default;

    public:
        inline WindowsPath& operator=(const WindowsPath& path) = default;
        inline WindowsPath& operator=(WindowsPath&& path) = default;

        inline WindowsPath& operator=(StringType&& source)    { m_String = Move(source); return *this; }
        inline WindowsPath& operator=(const ViewType source)  { m_String = source; return *this; }
        inline WindowsPath& operator=(const CharType* source) { m_String = source; return *this; }

    public:
        inline WindowsPath& operator+=(const WindowsPath& path) { m_String += path; return *this; }
        inline WindowsPath& operator+=(const ViewType path)     { m_String += path; return *this; }
        inline WindowsPath& operator+=(const CharType* path)    { m_String += path; return *this; }
        inline WindowsPath& operator+=(CharType ch)             { m_String += ch;   return *this; }

        inline WindowsPath operator+(const WindowsPath& path) { auto copy = *this; return (copy += path); }
        inline WindowsPath operator+(const ViewType path)     { auto copy = *this; return (copy += path); }
        inline WindowsPath operator+(const CharType* path)    { auto copy = *this; return (copy += path); }
        inline WindowsPath operator+(CharType ch)             { auto copy = *this; return (copy += ch);   }

    public:
        WindowsPath GetDrivePath() const;
        WindowsPath GetRootPath() const;

        WindowsPath GetRelativePath() const;
        WindowsPath GetParentPath() const;

        inline WindowsPath GetAnchorPath() const
        {
            return GetDrivePath() + GetRootPath();
        }

    public:
        inline bool IsEmpty() const { return m_String.IsEmpty(); }

        inline bool IsAbsolute() const { return (HasDrivePath() || HasRootPath()); }
        inline bool IsRelative() const { return !IsAbsolute(); }

        inline bool HasDrivePath()    const { return !GetDrivePath().IsEmpty(); }
        inline bool HasRootPath()     const { return !GetRootPath().IsEmpty(); }

        inline bool HasRelativePath() const { return !GetRelativePath().IsEmpty(); }
        inline bool HasParentPath()   const { return !GetParentPath().IsEmpty(); }

        inline bool HasAnchorPath()   const { return !GetAnchorPath().IsEmpty(); }

    public:
        inline const NativeChar* Raw() const { return m_String.Raw(); }
        inline const StringType& Native() const { return m_String; }

        operator StringType() const { return m_String; }

    public:
        inline void Clear() { return m_String.Clear(); }

    private:
        WindowsPath::ViewType GetDriveSubstring() const;
        WindowsPath::ViewType GetRootSubstring() const;
        WindowsPath::ViewType GetRelativeSubstring() const;

    private:
        StringType m_String;
    };
}
