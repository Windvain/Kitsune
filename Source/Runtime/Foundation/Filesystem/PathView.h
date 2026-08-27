#pragma once

#include "Foundation/String/String.h"
#include "Foundation/String/TranscodePresets.h"

namespace Kitsune::Filesystem
{
    class PathView
    {
    public:
        using ValueType = char;

#if defined(KITSUNE_OS_WINDOWS)
        using EncodingType = UTF8Encoding<char>;
        using NativeEncodingType = UTF16Encoding<wchar_t>;
#endif

    public:
        inline PathView() = default;
        inline PathView(std::nullptr_t) = delete;

        inline PathView(const char* string)
            : m_Path(string)
        {
        }

        inline PathView(const char* string, Usize size)
            : m_Path(string, size)
        {
        }

        inline PathView(StringView string)
            : m_Path(string)
        {
        }

        template<RandomAccessIterator Iter>
        inline PathView(Iter begin, Iter end)
            : m_Path(begin, end)
        {
        }

        PathView(const PathView&) = default;
        ~PathView() = default;

    public:
        PathView& operator=(const PathView&) = default;

        inline operator StringView() const
        {
            return m_Path;
        }

    public:
        [[nodiscard]]
        inline Usize Size() const
        {
            return m_Path.Size();
        }

        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return m_Path.IsEmpty();
        }

    public:
        [[nodiscard]]
        inline const char* Data() const
        {
            return m_Path.Data();
        }

        [[nodiscard]]
        inline StringView View() const
        {
            return m_Path;
        }

        [[nodiscard]]
        inline NativeString Native() const
        {
            return TranscodeString<EncodingType, NativeEncodingType>(m_Path);
        }

    public:
        [[nodiscard]] KITSUNE_API bool IsValidPath() const;

        [[nodiscard]] KITSUNE_API bool IsAbsolute() const;
        [[nodiscard]] KITSUNE_API bool IsRelative() const;

        [[nodiscard]] KITSUNE_API PathView GetRootName() const;
        [[nodiscard]] KITSUNE_API PathView GetRootDirectory() const;

        [[nodiscard]] KITSUNE_API PathView GetRootPath() const;
        [[nodiscard]] KITSUNE_API PathView GetRelativePath() const;

        [[nodiscard]] KITSUNE_API PathView GetParentPath() const;

        [[nodiscard]] KITSUNE_API PathView GetFilename() const;
        [[nodiscard]] KITSUNE_API PathView GetFileStem() const;
        [[nodiscard]] KITSUNE_API PathView GetExtension() const;

    public:
        inline void Swap(PathView& path)
        {
            m_Path.Swap(path.m_Path);
        }

    private:
        StringView m_Path;
    };

    inline bool operator==(const PathView& lhs, const PathView& rhs)
    {
        return (lhs.View() == rhs.View());
    }
}
