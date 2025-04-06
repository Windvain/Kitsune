#pragma once

#include "Foundation/String/CharTraits.h"
#include "Foundation/Concepts/Character.h"

#include "Foundation/Iterators/ReverseIterator.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    template<Character T>
    class BasicStringView
    {
    private:
        using ThisCharTraits = CharTraits<T>;

    public:
        using ValueType = T;

        using Iterator = const T*;
        using ConstIterator = const T*;

        using ReverseIterator = Kitsune::ReverseIterator<Iterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        BasicStringView() = default;
        BasicStringView(std::nullptr_t) = delete;


        BasicStringView(const T* str)
            : m_Pointer(str), m_Size(ThisCharTraits::Length(str))
        {
        }

        BasicStringView(const T* str, Usize size)
            : m_Pointer(str), m_Size(size)
        {
        }

        template<RandomAccessIterator It>
        BasicStringView(It begin, It end)
            : m_Pointer(begin), m_Size(end - begin)
        {
        }

        BasicStringView(const BasicStringView&) = default;

    public:
        BasicStringView& operator=(const BasicStringView&) = default;

    public:
        const T& operator[](Index index) const
        {
            if (index >= Size())
                throw OutOfRangeException();

            return m_Pointer[index];
        }

    public:
        [[nodiscard]] const T& Front() const
        {
            if (m_Pointer == nullptr)
                throw OutOfRangeException();

            return m_Pointer[0];
        }

        [[nodiscard]] const T& Back() const
        {
            if (m_Pointer == nullptr)
                throw OutOfRangeException();

            return m_Pointer[m_Size - 1];
        }

        [[nodiscard]] const T* Data() const { return m_Pointer; }

    public:
        [[nodiscard]] Usize Size()   const { return m_Size; }
        [[nodiscard]] bool IsEmpty() const { return (m_Size == 0); }

    public:
        void RemovePrefix(Usize n)
        {
            if (n > Size())
                throw OutOfRangeException();

            m_Pointer += n;
            m_Size -= n;
        }

        void RemoveSuffix(Usize n)
        {
            if (n > Size())
                throw OutOfRangeException();

            m_Size -= n;
        }

        [[nodiscard]]
        BasicStringView Substring(Usize startPos = 0, Usize count = 0) const
        {
            if (startPos > Size())
                throw OutOfRangeException();

            return BasicStringView(m_Pointer + startPos, KITSUNE_MIN(count, Size() - startPos));
        }

    public:
        [[nodiscard]] Iterator GetBegin()            { return m_Pointer; }
        [[nodiscard]] ConstIterator GetBegin() const { return m_Pointer; }

        [[nodiscard]] Iterator GetEnd()            { return (m_Pointer + m_Size); }
        [[nodiscard]] ConstIterator GetEnd() const { return (m_Pointer + m_Size); }

        [[nodiscard]] ReverseIterator GetReverseBegin()            { return ReverseIterator(GetEnd()); }
        [[nodiscard]] ReverseConstIterator GetReverseBegin() const { return ReverseIterator(GetEnd()); }

        [[nodiscard]] ReverseIterator GetReverseEnd()            { return ReverseIterator(GetBegin()); }
        [[nodiscard]] ReverseConstIterator GetReverseEnd() const { return ReverseIterator(GetBegin()); }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        Iterator begin() { return GetBegin(); }
        Iterator begin() const { return GetBegin(); }

        Iterator end() { return GetEnd(); }
        Iterator end() const { return GetEnd(); }

    private:
        const T* m_Pointer = nullptr;
        Usize m_Size = 0;
    };

    template<Character T>
    bool operator==(const BasicStringView<T>& str1, const BasicStringView<T>& str2)
    {
        return (str1.Size() == str2.Size()) &&
               (CharTraits<T>::Compare(str1.Data(), str2.Data(), str1.Size()) == 0);
    }

    template<Character T>
    bool operator==(const BasicStringView<T>& str1, const T* str2)
    {
        return (str1 == BasicStringView<T>(str2));
    }

    using StringView = BasicStringView<char>;
    using WideStringView = BasicStringView<wchar_t>;

    using U8StringView = BasicStringView<char8_t>;
    using U16StringView = BasicStringView<char16_t>;
    using U32StringView = BasicStringView<char32_t>;
}
