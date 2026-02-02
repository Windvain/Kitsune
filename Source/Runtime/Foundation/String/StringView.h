#pragma once

#include <cwchar>
#include <cstring>

#include "Foundation/Common/Macros.h"
#include "Foundation/Algorithms/Find.h"

#include "Foundation/Templates/Swap.h"
#include "Foundation/Concepts/Character.h"

#include "Foundation/Iterators/ReverseIterator.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    // Provides a view to a contiguous block of characters.
    template<Character T>
    class BasicStringView
    {
    public:
        static_assert(std::is_trivial_v<T>,
                    "BasicStringView<T> assumes that the element type being "
                    "passed to it is a trivial type.");

        using ValueType = T;

        using Iterator = const T*;
        using ConstIterator = const T*;

        using ReverseIterator = Kitsune::ReverseIterator<Iterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline BasicStringView() = default;
        inline BasicStringView(std::nullptr_t) = delete;

        inline BasicStringView(const T* string)
            : m_Pointer(string), m_Size(CalculateStringSize(string))
        {
        }

        inline BasicStringView(const T* string, Usize size)
            : m_Pointer(string), m_Size(size)
        {
        }

        template<RandomAccessIterator It>
        inline BasicStringView(It begin, It end)
            : m_Pointer(ToAddress(begin)), m_Size(end - begin)
        {
        }

        BasicStringView(const BasicStringView&) = default;

    public:
        BasicStringView& operator=(const BasicStringView&) = default;

    public:
        inline const T& operator[](Index index) const
        {
            if (index >= Size())
                throw OutOfRangeException();

            return m_Pointer[index];
        }

    public:
        [[nodiscard]]
        inline const T& Front() const
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return m_Pointer[0];
        }

        [[nodiscard]]
        inline const T& Back() const
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return m_Pointer[m_Size - 1];
        }

        [[nodiscard]]
        inline const T* Data() const
        {
            return m_Pointer;
        }

    public:
        [[nodiscard]]
        inline Usize Size() const
        {
            return m_Size;
        }

        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return (m_Size == 0);
        }

    public:
        inline void RemovePrefix(Usize offset)
        {
            if (offset > Size())
                throw OutOfRangeException();

            m_Pointer += offset;
            m_Size -= offset;
        }

        inline void RemoveSuffix(Usize offset)
        {
            if (offset > Size())
                throw OutOfRangeException();

            m_Size -= offset;
        }

        inline void Swap(BasicStringView<T>& string)
        {
            Kitsune::Swap(m_Pointer, string.m_Pointer);
            Kitsune::Swap(m_Size, string.m_Size);
        }

    public:
        [[nodiscard]]
        inline bool StartsWith(BasicStringView<T> string) const
        {
            Usize minimumSize = KITSUNE_MIN(Size(), string.Size());
            return (BasicStringView<T>(Data(), minimumSize) == string);
        }

        [[nodiscard]]
        inline bool StartsWith(T character) const
        {
            return (!IsEmpty() && (Front() == character));
        }

        [[nodiscard]]
        inline bool StartsWith(const T* string) const
        {
            return StartsWith(BasicStringView<T>(string));
        }

        [[nodiscard]]
        inline bool EndsWith(BasicStringView<T> string)
        {
            return (Size() >= string.Size()) &&
                   (BasicStringView<T>(m_Pointer + (Size() - string.Size())) == string);
        }

        [[nodiscard]]
        inline bool EndsWith(const T character)
        {
            return (!IsEmpty() && (Back() == character));
        }

        [[nodiscard]]
        inline bool EndsWith(const T* string)
        {
            return EndsWith(BasicStringView<T>(string));
        }

        [[nodiscard]]
        inline bool Contains(BasicStringView<T> string)
        {
            auto iterator = Algorithms::Find(
                GetBegin(), GetEnd(), string.GetBegin(), string.GetEnd());

            return (iterator != GetEnd());
        }

        [[nodiscard]]
        inline bool Contains(T character)
        {
            auto iterator = Algorithms::Find(GetBegin(), GetEnd(), character);
            return (iterator != GetEnd());
        }

        [[nodiscard]]
        inline bool Contains(const T* string)
        {
            return Contains(BasicStringView<T>(string));
        }

        [[nodiscard]]
        inline ConstIterator Find(BasicStringView<T> string) const
        {
            return Algorithms::Find(GetBegin(), GetEnd(),
                                    string.GetBegin(), string.GetEnd());
        }

        [[nodiscard]]
        inline ConstIterator Find(T character) const
        {
            return Algorithms::Find(GetBegin(), GetEnd(), character);
        }

        [[nodiscard]]
        inline BasicStringView Substring(Index startPos, Usize count) const
        {
            if (startPos > Size())
                throw OutOfRangeException();

            Usize minimum = KITSUNE_MIN(count, Size() - startPos);
            return BasicStringView(m_Pointer + startPos, minimum);
        }

    public:
        [[nodiscard]] inline Iterator GetBegin() { return m_Pointer; }
        [[nodiscard]] inline ConstIterator GetBegin() const { return m_Pointer; }

        [[nodiscard]] inline Iterator GetEnd() { return (m_Pointer + m_Size); }
        [[nodiscard]] inline ConstIterator GetEnd() const { return (m_Pointer + m_Size); }

        [[nodiscard]]
        inline ReverseIterator GetReverseBegin()
        {
            return ReverseIterator(GetEnd());
        }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseBegin() const
        {
            return ReverseIterator(GetEnd());
        }

        [[nodiscard]]
        inline ReverseIterator GetReverseEnd()
        {
            return ReverseIterator(GetBegin());
        }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseEnd() const
        {
            return ReverseIterator(GetBegin());
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline Iterator begin() { return GetBegin(); }
        inline Iterator begin() const { return GetBegin(); }

        inline Iterator end() { return GetEnd(); }
        inline Iterator end() const { return GetEnd(); }

    private:
        inline static Usize CalculateStringSize(const T* string)
        {
            if constexpr (std::is_same_v<T, char>)
                return std::strlen(string);
            else if constexpr (std::is_same_v<T, wchar_t>)
                return std::wcslen(string);
            else
            {
                Usize size = 0;
                for (; *string != T(); ++string, ++size);

                return size;
            }
        }

    private:
        const T* m_Pointer = nullptr;
        Usize m_Size = 0;
    };

    template<Character T>
    inline bool operator==(const BasicStringView<T>& string1,
                           const BasicStringView<T>& string2)
    {
        return (string1.Size() == string2.Size()) &&
               (std::memcmp(string1.Data(), string2.Data(),
                            string1.Size() * sizeof(T)) == 0);
    }

    template<Character T>
    inline bool operator==(const BasicStringView<T>& string1, const T* string2)
    {
        return (string1 == BasicStringView<T>(string2));
    }

    template<Character T>
    inline bool operator==(const T* string1, const BasicStringView<T>& string2)
    {
        return (string2 == string1);
    }

    using StringView = BasicStringView<char>;
    using WideStringView = BasicStringView<wchar_t>;

    using NativeStringView = BasicStringView<NativeChar>;

    using U8StringView = BasicStringView<char8_t>;
    using U16StringView = BasicStringView<char16_t>;
    using U32StringView = BasicStringView<char32_t>;
}
