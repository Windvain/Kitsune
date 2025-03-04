#pragma once

#include "Foundation/String/String.h"

namespace Kitsune
{
    class WindowsPath
    {
    public:
        using CharType = wchar_t;

        using StringType = WideString;
        using ViewType = WideStringView;

        using Iterator = StringType::Iterator;
        using ConstIterator = StringType::ConstIterator;

        using ReverseIterator = StringType::ReverseIterator;
        using ReverseConstIterator = StringType::ReverseConstIterator;

        static constexpr wchar_t Seperator = L'\\';

    public:
        inline WindowsPath() : m_String() { /* ... */ }

        inline WindowsPath(const wchar_t* path) : m_String(path) { /* ... */ }
        inline WindowsPath(const ViewType path) : m_String(path) { /* ... */ }

        inline WindowsPath(StringType&& path)   : m_String(path) { /* ... */ }

        inline WindowsPath(const WindowsPath&) = default;
        inline WindowsPath(WindowsPath&& path) = default;

        inline ~WindowsPath() = default;

    public:
        inline WindowsPath& operator=(const WindowsPath&) = default;
        inline WindowsPath& operator=(WindowsPath&&) = default;

    public:
        inline StringType& Native()             { return m_String; }
        inline const StringType& Native() const { return m_String; }

    public:
        inline void Clear() { m_String.Clear(); }

    public:
        KITSUNE_API_ bool Exists() const;

        KITSUNE_API_ bool IsFile() const;
        KITSUNE_API_ bool IsDirectory() const;

        KITSUNE_API_ bool IsAbsolute() const;
        inline bool IsRelative() const { return !IsAbsolute(); }

    private:
        WideString m_String;
    };

    inline bool operator==(const WindowsPath& path1, const WindowsPath& path2)
    {
        return (path1.Native() == path2.Native());
    }
}
