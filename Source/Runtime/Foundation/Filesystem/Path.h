#pragma once

#include "Foundation/Filesystem/PathView.h"

namespace Kitsune::Filesystem
{
    class KITSUNE_API Path
    {
    public:
        using ValueType = typename PathView::ValueType;

        using EncodingType = typename PathView::EncodingType;
        using NativeEncodingType = typename PathView::NativeEncodingType;

    public:
        inline Path() = default;
        inline Path(std::nullptr_t) = delete;

        template<typename StrvLike>
            requires std::is_convertible_v<StrvLike, StringView>
        inline Path(const StrvLike& string)
            : m_Path(StringView(string))
        {
        }

        inline Path(String&& string)
            : m_Path(Move(string))
        {
        }

        inline Path(const Path&) = default;
        inline Path(Path&&) = default;

        inline ~Path() = default;

    public:
        Path& operator=(const Path&) = default;
        Path& operator=(Path&&) = default;

        inline Path& operator=(String&& string)
        {
            m_Path = Move(string);
            return *this;
        }

        template<typename StrvLike>
            requires std::is_convertible_v<StrvLike, StringView>
        inline Path& operator=(const StrvLike& string)
        {
            m_Path = string;
            return *this;
        }

    public:
        inline operator String() const
        {
            return m_Path;
        }

        inline operator StringView() const
        {
            return m_Path;
        }

        inline operator PathView() const
        {
            return PathView(m_Path);
        }

    public:
        inline Path& operator+=(const Path& path)
        {
            m_Path += path.String();
            return *this;
        }

        template<typename StrvLike>
            requires std::is_convertible_v<StrvLike, StringView>
        inline Path& operator+=(const StrvLike& string)
        {
            m_Path += string;
            return *this;
        }

        inline Path operator+(const Path& path) const
        {
            Path copy = *this;
            return (copy += path);
        }

        template<typename StrvLike>
            requires std::is_convertible_v<StrvLike, StringView>
        inline Path operator+(const StrvLike& string) const
        {
            Path copy = *this;
            return (copy += string);
        }

    public:
        Path& operator/=(const Path& path);

        template<typename StrvLike>
            requires std::is_convertible_v<StrvLike, StringView>
        inline Path& operator/=(const StrvLike& string)
        {
            return operator/=(Path(string));
        }

        inline Path operator/(const Path& path) const
        {
            Path copy = *this;
            return (copy /= path);
        }

        template<typename StrvLike>
            requires std::is_convertible_v<StrvLike, StringView>
        inline Path operator/(const StrvLike& string) const
        {
            Path copy = *this;
            return (copy /= string);
        }

    public:
        [[nodiscard]]
        inline bool IsValidPath() const
        {
            return PathView(m_Path).IsValidPath();
        }

        [[nodiscard]]
        inline bool IsAbsolute() const
        {
            return PathView(m_Path).IsAbsolute();
        }

        [[nodiscard]]
        inline bool IsRelative() const
        {
            return PathView(m_Path).IsRelative();
        }

    public:
        [[nodiscard]]
        inline PathView GetRootName() const
        {
            return PathView(m_Path).GetRootName();
        }

        [[nodiscard]]
        inline PathView GetRootDirectory() const
        {
            return PathView(m_Path).GetRootDirectory();
        }

        [[nodiscard]]
        inline PathView GetRootPath() const
        {
            return PathView(m_Path).GetRootPath();
        }

        [[nodiscard]]
        inline PathView GetRelativePath() const
        {
            return PathView(m_Path).GetRelativePath();
        }

        [[nodiscard]]
        inline PathView GetParentPath() const
        {
            return PathView(m_Path).GetParentPath();
        }

        [[nodiscard]]
        inline PathView GetFilename() const
        {
            return PathView(m_Path).GetFilename();
        }

        [[nodiscard]]
        inline PathView GetFileStem() const
        {
            return PathView(m_Path).GetFileStem();
        }

        [[nodiscard]]
        inline PathView GetExtension() const
        {
            return PathView(m_Path).GetExtension();
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
        inline const char* Raw() const
        {
            return m_Path.Raw();
        }

        [[nodiscard]]
        inline const String& String() const
        {
            return m_Path;
        }

        [[nodiscard]]
        inline NativeString Native() const
        {
            return PathView(m_Path).Native();
        }

    public:
        inline void Clear()
        {
            m_Path.Clear();
        }

        inline void Swap(Path& path)
        {
            m_Path.Swap(path.m_Path);
        }

    public:
        void MakePreferredSeperator();

        inline void RemoveExtension()
        {
            ReplaceExtension(PathView());
        }

        inline void RemoveFilename()
        {
            ReplaceFilename(PathView());
        }

        inline void ReplaceExtension(PathView path)
        {
            StringView extension = GetExtension();
            if (!extension.IsEmpty())
            {
                m_Path.Remove(extension.GetBegin(), extension.GetEnd());
                operator+=(path);
            }
        }

        inline void ReplaceFilename(PathView path)
        {
            StringView filename = GetFilename();
            if (!filename.IsEmpty())
            {
                m_Path.Remove(filename.GetBegin(), filename.GetEnd());
                operator/=(path);
            }
        }

    private:
        Kitsune::String m_Path;
    };

    inline bool operator==(const Path& lhs, const Path& rhs)
    {
        return (lhs.String() == rhs.String());
    }
}
