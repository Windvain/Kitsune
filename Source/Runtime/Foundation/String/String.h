#pragma once

#include <initializer_list>

#include "Foundation/Templates/Swap.h"
#include "Foundation/String/StringView.h"

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Algorithms/Distance.h"
#include "Foundation/Algorithms/Uninitialized.h"

namespace Kitsune
{
    // Container that keeps a contiguous sequence of characters.
    template<Character T, Allocator Alloc = GlobalAllocator>
    class BasicString
    {
    private:
        static_assert(
            std::is_trivial_v<T>,
            "BasicString<T> assumes that the element type being passed to "
            "it is a trivial type.");

    public:
        using ValueType = T;
        using AllocatorType = Alloc;

        using Iterator = T*;
        using ConstIterator = const T*;

        using ReverseIterator = Kitsune::ReverseIterator<Iterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline BasicString()
            : BasicString(/* Capacity: */ Usize(0), Alloc())
        {
        }

        inline explicit BasicString(const Alloc& allocator)
            : BasicString()
        {
            m_Allocator = allocator;
        }

        inline explicit BasicString(Usize capacity,
                                    const Alloc& allocator = Alloc())
            : m_Size(0), m_Allocator(allocator)
        {
            if (capacity < s_SmallBufferSize)
                m_Pointer = m_SharedData.Buffer;
            else
            {
                // No need to align allocations, all character types should
                // be trivial and therefore align to alignof(std::max_align_t).
                m_SharedData.Capacity = s_AllocationFactor * capacity;
                m_Pointer = static_cast<T*>(
                    m_Allocator.Allocate((m_SharedData.Capacity + 1) * sizeof(T)));
            }

            // Close it off with a null terminator.
            *m_Pointer = T();
        }

        inline BasicString(Usize count, T character,
                           const Alloc& allocator = Alloc())
            : BasicString(count, allocator)
        {
            // Optimization for char types, std::memset() is usually converted to
            // a form of __builtin_memset(), which is faster
            // than UninitializedFill().
            if constexpr (!std::is_same_v<T, char>)
                *Algorithms::UninitializedFillN(m_Pointer, count, character) = T();
            else
            {
                std::memset(m_Pointer, character, count);
                m_Pointer[count] = T();
            }

            m_Size = count;
        }

        inline BasicString(const T* string, Usize size,
                           const Alloc& allocator = Alloc())
            : BasicString(size, allocator)
        {
            std::memcpy(m_Pointer, string, size * sizeof(T));

            m_Pointer[size] = T();
            m_Size = size;
        }

        inline BasicString(const T* string, const Alloc& allocator = Alloc())
            : BasicString(BasicStringView<T>(string), allocator)
        {
        }

        template<ForwardIterator Iter>
        inline BasicString(Iter begin, Iter end,
                           const Alloc& allocator = Alloc())
            : BasicString(Algorithms::Distance(begin, end), allocator)
        {
            *Algorithms::UninitializedCopy(begin, end, m_Pointer) = T();
            m_Size = Algorithms::Distance(begin, end);
        }

        inline BasicString(const BasicString& string)
            : BasicString(string.Raw(), string.Size(), string.GetAllocator())
        {
        }

        inline BasicString(BasicString&& string)
            : m_Allocator(Move(string.GetAllocator()))
        {
            m_Size = string.Size();
            m_SharedData = string.m_SharedData;

            m_Pointer = string.IsStorageLocal() ?
                m_SharedData.Buffer :
                string.m_Pointer;

            // Leave the moved string in a state where IsEmpty() is true.
            string.m_Pointer = string.m_SharedData.Buffer;
            string.m_Size = 0;
        }

        inline BasicString(std::initializer_list<T> initList,
                           const Alloc& allocator = Alloc())
            : BasicString(initList.begin(), initList.end(), allocator)
        {
        }

        inline explicit BasicString(BasicStringView<T> string,
                                    const Alloc& allocator = Alloc())
            : BasicString(string.Data(), string.Size(), allocator)
        {
        }

        inline ~BasicString()
        {
            Reset();
        }

    public:
        inline BasicString& operator=(const BasicString& string)
        {
            if (this == &string)
                return *this;

            if (m_Allocator != string.GetAllocator())
                Reset();

            m_Allocator = string.GetAllocator();
            return operator=(BasicStringView<T>(string.Raw(), string.Size()));
        }

        inline BasicString& operator=(BasicString&& string)
        {
            if (this == &string)
                return *this;

            Reset();
            m_Allocator = Move(string.GetAllocator());

            m_Size = string.Size();
            m_SharedData = string.m_SharedData;

            m_Pointer = string.IsStorageLocal() ?
                m_SharedData.Buffer : string.m_Pointer;

            string.m_Pointer = string.m_SharedData.Buffer;
            string.m_Size = 0;

            return *this;
        }

        inline BasicString& operator=(const T* string)
        {
            return operator=(BasicStringView<T>(string));
        }

        inline BasicString& operator=(T character)
        {
            return operator=(BasicStringView<T>(&character, 1));
        }

        inline BasicString& operator=(std::initializer_list<T> initList)
        {
            Assign(initList.begin(), initList.end());
            return *this;
        }

        inline BasicString& operator=(BasicStringView<T> string)
        {
            if (string.Size() > Capacity())
                BasicString(string.Size(), Move(m_Allocator)).Swap(*this);

            m_Size = string.Size();
            std::memcpy(m_Pointer, string.Data(), m_Size * sizeof(T));

            m_Pointer[string.Size()] = T();
            return *this;
        }

    public:
        inline T& operator[](Index index)
        {
            if (index >= Size())
                throw OutOfRangeException();

            return Data()[index];
        }

        inline const T& operator[](Index index) const
        {
            if (index >= Size())
                throw OutOfRangeException();

            return Data()[index];
        }

        inline operator BasicStringView<T>() const
        {
            return BasicStringView<T>(Data(), Size());
        }

        inline BasicString& operator+=(const BasicString& string)
        {
            Append(string.Raw(), string.Size());
            return *this;
        }

        inline BasicString& operator+=(T character)
        {
            Append(1, character);
            return *this;
        }

        inline BasicString& operator+=(const T* string)
        {
            return operator+=(BasicStringView<T>(string));
        }

        inline BasicString& operator+=(std::initializer_list<T> initList)
        {
            Append(initList.begin(), initList.end());
            return *this;
        }

        inline BasicString& operator+=(BasicStringView<T> string)
        {
            Append(string.Data(), string.Size());
            return *this;
        }

        inline BasicString operator+(const BasicString& string)
        {
            BasicString copy = *this;
            copy += string;

            return copy;
        }

        inline BasicString operator+(T character)
        {
            BasicString copy = *this;
            copy += character;

            return copy;
        }

        inline BasicString operator+(const T* string)
        {
            BasicString copy = *this;
            copy += string;

            return copy;
        }

        inline BasicString operator+(BasicStringView<T> string)
        {
            BasicString copy = *this;
            copy += string;

            return copy;
        }

    public:
        [[nodiscard]]
        inline T& Front()
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return *Data();
        }

        [[nodiscard]]
        inline const T& Front() const
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return *Data();
        }

        [[nodiscard]]
        inline T& Back()
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return Data()[Size() - 1];
        }

        [[nodiscard]]
        inline const T& Back() const
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return Data()[Size() - 1];
        }

        [[nodiscard]]
        inline const T* Raw() const
        {
            return Data();
        }

        [[nodiscard]] inline T* Data() { return m_Pointer; }
        [[nodiscard]] inline const T* Data() const { return m_Pointer; }

    public:
        [[nodiscard]]
        inline Usize Size() const
        {
            return m_Size;
        }

        [[nodiscard]]
        inline Usize Capacity() const
        {
            if (IsStorageLocal())
                return s_SmallBufferSize - 1;

            return m_SharedData.Capacity;
        }

        [[nodiscard]]
        inline static Usize GetLocalCapacity()
        {
            return s_SmallBufferSize - 1;
        }

        [[nodiscard]]
        inline bool IsStorageLocal() const
        {
            return (m_Pointer == m_SharedData.Buffer);
        }

        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return (Size() == 0);
        }

        [[nodiscard]]
        inline Alloc& GetAllocator()
        {
            return m_Allocator;
        }

        [[nodiscard]]
        inline const Alloc& GetAllocator() const
        {
            return m_Allocator;
        }

    public:
        [[nodiscard]] inline Iterator GetBegin() { return m_Pointer; }
        [[nodiscard]] inline ConstIterator GetBegin() const { return m_Pointer; }

        [[nodiscard]]
        inline Iterator GetEnd()
        {
            return (m_Pointer + m_Size);
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return (m_Pointer + m_Size);
        }

        [[nodiscard]]
        inline ReverseIterator GetReverseBegin()
        {
            return ReverseIterator(GetEnd());
        }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseBegin() const
        {
            return ReverseConstIterator(GetEnd());
        }

        [[nodiscard]]
        inline ReverseIterator GetReverseEnd()
        {
            return ReverseIterator(GetBegin());
        }

        [[nodiscard]]
        inline ReverseConstIterator GetReverseEnd() const
        {
            return ReverseConstIterator(GetBegin());
        }

    public:
        inline void Assign(Usize count, T character)
        {
            if (count > Capacity())
                BasicString(count, Move(m_Allocator)).Swap(*this);

            m_Size = count;
            *Algorithms::UninitializedFillN(m_Pointer, count, character) = T();
        }

        template<ForwardIterator Iter>
        inline void Assign(Iter begin, Iter end)
        {
            auto distance = Algorithms::Distance(begin, end);
            if (distance > Capacity())
                BasicString(distance, Move(m_Allocator)).Swap(*this);

            m_Size = distance;
            *Algorithms::UninitializedCopy(begin, end, m_Pointer) = T();
        }

    public:
        inline void Clear()
        {
            m_Size = 0;
            m_Pointer[0] = T();
        }

        inline void Reset()
        {
            // No need to use Capacity(), we already know that the
            // string is allocated on the heap.
            if (!IsStorageLocal())
                m_Allocator.Free(m_Pointer, (m_SharedData.Capacity + 1) * sizeof(T));

            m_Pointer = m_SharedData.Buffer;
            Clear();
        }

        inline Iterator Insert(ConstIterator iter, const T* string)
        {
            return Insert(iter, BasicStringView<T>(string));
        }

        inline Iterator Insert(ConstIterator iter, const T* string, Usize size)
        {
            return Insert(iter, string, string + size);
        }

        inline Iterator Insert(ConstIterator iter, BasicStringView<T> string)
        {
            return Insert(iter, string.GetBegin(), string.GetEnd());
        }

        inline Iterator Insert(ConstIterator iter, T character)
        {
            return Insert(iter, BasicStringView<T>(&character, 1));
        }

        inline Iterator Insert(ConstIterator iter, Usize count, T character)
        {
            if ((iter < GetBegin()) || (iter > GetEnd()))
                throw OutOfRangeException();

            Index index = iter - GetBegin();
            Usize newSize = Size() + count;

            if (newSize > Capacity())
                Reserve(newSize);

            Iterator position = GetBegin() + index;
            std::memmove(
                position + count,
                position,
                (GetEnd() - position + 1) * sizeof(T));

            Algorithms::UninitializedFillN(position, count, character);
            m_Size = newSize;

            return position;
        }

        inline Iterator Insert(ConstIterator iter, const BasicString& string)
        {
            return Insert(iter, string.GetBegin(), string.GetEnd());
        }

        template<ForwardIterator Iter>
        inline Iterator Insert(ConstIterator iter, Iter begin, Iter end)
        {
            if ((iter < GetBegin()) || (iter > GetEnd()))
                throw OutOfRangeException();

            auto count = Algorithms::Distance(begin, end);
            Index index = iter - GetBegin();

            Usize newSize = Size() + count;
            if (newSize > Capacity())
                Reserve(newSize);

            Iterator position = GetBegin() + index;
            std::memmove(
                position + count,
                position,
                (GetEnd() - position + 1) * sizeof(T));

            // Small optimization for contiguous buffers.
            if constexpr (AnyOf<Iter, T*, const T*>)
                std::memcpy(position, begin, count * sizeof(T));
            else
                Algorithms::UninitializedCopy(begin, end, position);

            m_Size = newSize;
            return position;
        }

        inline Iterator Insert(ConstIterator iter, std::initializer_list<T> initList)
        {
            return Insert(iter, initList.begin(), initList.end());
        }

        inline void Remove(ConstIterator iter)
        {
            Remove(iter, iter + 1);
        }

        inline void Remove(ConstIterator begin, ConstIterator end)
        {
            if ((begin < GetBegin()) || (begin >= GetEnd()) || (end < GetBegin()) ||
                (end > GetEnd()))
            {
                throw OutOfRangeException();
            }

            std::memmove(
                Data() + (begin - GetBegin()),      // == begin.
                end,
                (GetEnd() - end + 1) * sizeof(T));

            m_Size -= (end - begin);
        }

        inline void PushBack(T character)
        {
            Usize newSize = Size() + 1;
            if (Capacity() < newSize)
                Reserve(newSize);

            *GetEnd() = character;
            *(GetEnd() + 1) = T();

            ++m_Size;
        }

        inline void PopBack()
        {
            if (IsEmpty())
                throw OutOfRangeException();

            --m_Size;
            *GetEnd() = T();
        }

        inline void Append(Usize count, T character)
        {
            Usize newSize = Size() + count;
            if (Capacity() < newSize)
                Reserve(newSize);

            *Algorithms::UninitializedFillN(GetEnd(), count, character) = T();
            m_Size = newSize;
        }

        inline void Append(const T* string, Usize size)
        {
            Usize newSize = Size() + size;
            if (Capacity() < newSize)
                Reserve(newSize);

            std::memcpy(GetEnd(), string, size * sizeof(T));
            Data()[newSize] = T();

            m_Size = newSize;
        }

        template<ForwardIterator Iter>
        inline void Append(Iter begin, Iter end)
        {
            Usize newSize = Size() + Algorithms::Distance(begin, end);
            if (Capacity() < newSize)
                Reserve(newSize);

            *Algorithms::UninitializedCopy(begin, end, GetEnd()) = T();
            m_Size = newSize;
        }

        [[nodiscard]]
        inline bool StartsWith(BasicStringView<T> string) const
        {
            return BasicStringView<T>(*this).StartsWith(string);
        }

        [[nodiscard]]
        inline bool StartsWith(T character) const
        {
            return BasicStringView<T>(*this).StartsWith(character);
        }

        [[nodiscard]]
        inline bool StartsWith(const T* string) const
        {
            return BasicStringView<T>(*this).StartsWith(string);
        }

        [[nodiscard]]
        inline bool EndsWith(BasicStringView<T> string) const
        {
            return BasicStringView<T>(*this).EndsWith(string);
        }

        [[nodiscard]]
        inline bool EndsWith(T character) const
        {
            return BasicStringView<T>(*this).EndsWith(character);
        }

        [[nodiscard]]
        inline bool EndsWith(const T* string) const
        {
            return BasicStringView<T>(*this).EndsWith(string);
        }

        [[nodiscard]]
        inline bool Contains(BasicStringView<T> string) const
        {
            return BasicStringView<T>(*this).Contains(string);
        }

        [[nodiscard]]
        inline bool Contains(T character) const
        {
            return BasicStringView<T>(*this).Contains(character);
        }

        [[nodiscard]]
        inline bool Contains(const T* string) const
        {
            return BasicStringView<T>(*this).Contains(string);
        }

        [[nodiscard]]
        inline Iterator Find(BasicStringView<T> string)
        {
            return Algorithms::Find(
                GetBegin(), GetEnd(),
                string.GetBegin(), string.GetEnd());
        }

        [[nodiscard]]
        inline ConstIterator Find(BasicStringView<T> string) const
        {
            return Algorithms::Find(
                GetBegin(), GetEnd(),
                string.GetBegin(), string.GetEnd());
        }

        [[nodiscard]]
        inline Iterator Find(T character)
        {
            return Algorithms::Find(GetBegin(), GetEnd(), character);
        }

        [[nodiscard]]
        inline ConstIterator Find(T character) const
        {
            return Algorithms::Find(GetBegin(), GetEnd(), character);
        }

        [[nodiscard]]
        inline Iterator Find(const T* string)
        {
            return Find(BasicStringView<T>(string));
        }

        [[nodiscard]]
        inline ConstIterator Find(const T* string) const
        {
            return Find(BasicStringView<T>(string));
        }

        [[nodiscard]]
        inline BasicStringView<T> Substring(Index startPos, Usize count) const
        {
            return BasicStringView<T>(GetBegin() + startPos, count);
        }

        inline void Swap(BasicString& string)
        {
            // This is incredibly hard to read. Gosh.
            T* oldPointer = m_Pointer;
            m_Pointer = string.IsStorageLocal() ?
                m_SharedData.Buffer :
                string.m_Pointer;

            if (oldPointer == m_SharedData.Buffer)
                string.m_Pointer = string.m_SharedData.Buffer;
            else
                string.m_Pointer = oldPointer;

            Kitsune::Swap(m_SharedData, string.m_SharedData);
            Kitsune::Swap(m_Size, string.m_Size);
            Kitsune::Swap(m_Allocator, string.GetAllocator());
        }

    public:
        inline void Reserve(Usize newCapacity)
        {
            if (newCapacity <= Capacity())
                return;

            newCapacity *= s_AllocationFactor;

            T* pointer = static_cast<T*>(
                m_Allocator.Allocate((newCapacity + 1) * sizeof(T)));

            std::memcpy(pointer, Raw(), (Size() + 1) * sizeof(T));

            if (!IsStorageLocal())
                m_Allocator.Free(m_Pointer, (Capacity() + 1) * sizeof(T));

            m_SharedData.Capacity = newCapacity;
            m_Pointer = pointer;
        }

        inline void ShrinkToFit()
        {
            if (IsStorageLocal() || (Size() == Capacity()))
                return;

            if (Size() < s_SmallBufferSize)
            {
                Usize capacity = m_SharedData.Capacity;
                std::memcpy(
                    m_SharedData.Buffer, m_Pointer, (Size() + 1) * sizeof(T));

                m_Allocator.Free(m_Pointer, (capacity + 1) * sizeof(T));
                m_Pointer = m_SharedData.Buffer;
            }
            else
            {
                // No need to align allocations, all character types should
                // be trivial and therefore align to alignof(std::max_align_t).
                T* pointer = static_cast<T*>(
                    m_Allocator.Allocate((Size() + 1) * sizeof(T)));

                std::memcpy(pointer, m_Pointer, (Size() + 1) * sizeof(T));
                m_Allocator.Free(m_Pointer, (m_SharedData.Capacity + 1) * sizeof(T));

                m_Pointer = pointer;
                m_SharedData.Capacity = Size();
            }
        }

        inline void Resize(Usize count)
        {
            Resize(count, T());
        }

        inline void Resize(Usize count, T ch)
        {
            if (Size() == count)
                return;

            if (Size() < count)
                Insert(GetEnd(), count - Size(), ch);
            else
                Remove(GetBegin() + count, GetEnd());
        }

    public:
        BasicString(std::nullptr_t) = delete;
        BasicString& operator=(std::nullptr_t) = delete;

        // Should not be called by engine/client code.
        // Made public so that the compiler can generate range-based for loops.
        inline Iterator begin() { return GetBegin(); }
        inline ConstIterator begin() const { return GetBegin(); }

        inline Iterator end() { return GetEnd(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        static constexpr Usize s_SmallBufferSize = 16 / sizeof(T);
        static constexpr Usize s_AllocationFactor = 2;

        // The largest character currently is char32_t, which is 4 bytes
        // long.
        // That makes the minimum size 16 bytes / sizeof(char32_t) = 4 characters
        // for the entire buffer.
        static_assert(
            s_SmallBufferSize >= 4,
            "The BasicString<T> class has not been tested for"
            "characters above 4 bytes/32 bits in size.");

    private:
        T* m_Pointer;
        Usize m_Size;

        union
        {
            T Buffer[s_SmallBufferSize];
            Usize Capacity;
        } m_SharedData;

        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };

    template<Character T, Allocator Alloc1, Allocator Alloc2>
    inline bool operator==(const BasicString<T, Alloc1>& string1,
                           const BasicString<T, Alloc2>& string2)
    {
        return (string1.Size() == string2.Size()) &&
               (std::memcmp(string1.Raw(), string2.Raw(),
                            string1.Size() * sizeof(T)) == 0);
    }

    template<Character T, Allocator Alloc>
    inline bool operator==(const BasicString<T, Alloc>& string1, const T* string2)
    {
        BasicStringView<T> stringView = string2;
        return (stringView == string1.Raw());
    }

    template<Character T, Allocator Alloc>
    inline bool operator==(const T* string1, const BasicString<T, Alloc>& string2)
    {
        return (string2 == string1);
    }

    using String = BasicString<char>;
    using WideString = BasicString<wchar_t>;

    using NativeString = BasicString<NativeChar>;

    using U8String = BasicString<char8_t>;
    using U16String = BasicString<char16_t>;
    using U32String = BasicString<char32_t>;
}
