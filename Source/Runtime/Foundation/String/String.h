#pragma once

#include <cstring>
#include <type_traits>
#include <initializer_list>

#include "Foundation/Templates/Move.h"

#include "Foundation/String/StringView.h"
#include "Foundation/Concepts/Character.h"

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Algorithms/Swap.h"
#include "Foundation/Algorithms/Equal.h"
#include "Foundation/Algorithms/Distance.h"
#include "Foundation/Algorithms/Uninitialized.h"

namespace Kitsune
{
    template<Character T, Allocator Alloc = GlobalAllocator>
    class BasicString
    {
    private:
        static_assert(std::is_trivial_v<T>,
            "Template parameter `T` has to be a trivial type.");

        using ThisCharTraits = CharTraits<T>;

    public:
        using ValueType = T;
        using AllocatorType = Alloc;

        using ViewType = BasicStringView<T>;

        using Iterator = T*;
        using ConstIterator = const T*;

        using ReverseIterator = Kitsune::ReverseIterator<Iterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline BasicString() : m_Data() { /* ... */ }
        BasicString(std::nullptr_t) = delete;

        inline explicit BasicString(const Alloc& alloc)
            : m_Allocator(alloc)
        {
        }

        inline explicit BasicString(Alloc&& alloc)
            : m_Allocator(Move(alloc))
        {
        }

        inline explicit BasicString(Usize capacity, const Alloc& alloc)
            : m_Allocator(alloc)
        {
            InitializeCapacity(capacity);
        }

        inline explicit BasicString(Usize capacity, Alloc&& alloc = Alloc())
            : m_Allocator(alloc)
        {
            InitializeCapacity(capacity);
        }

        inline BasicString(Usize count, T ch, const Alloc& alloc)
            : BasicString(count, alloc)
        {
            *Algorithms::UninitializedFillN(m_Data.Pointer, count, ch) = T();
            m_Data.Size = count;
        }

        inline BasicString(Usize count, T ch, Alloc&& alloc = Alloc())
            : BasicString(count, Move(alloc))
        {
            *Algorithms::UninitializedFillN(m_Data.Pointer, count, ch) = T();
            m_Data.Size = count;
        }

        inline BasicString(const T* str, Usize size, const Alloc& alloc)
            : BasicString(size, alloc)
        {
            std::memcpy(m_Data.Pointer, str, size * sizeof(T));

            m_Data.Pointer[size] = T();
            m_Data.Size = size;
        }

        inline BasicString(const T* str, Usize size, Alloc&& alloc = Alloc())
            : BasicString(size, Move(alloc))
        {
            std::memcpy(m_Data.Pointer, str, size * sizeof(T));

            m_Data.Pointer[size] = T();
            m_Data.Size = size;
        }

        inline BasicString(const T* str, const Alloc& alloc)
            : BasicString(str, ThisCharTraits::Length(str), alloc)
        {
        }

        inline BasicString(const T* str, Alloc&& alloc = Alloc())
            : BasicString(str, ThisCharTraits::Length(str), Move(alloc))
        {
        }

        template<ForwardIterator It>
        inline BasicString(It begin, It end, const Alloc& alloc)
            : BasicString(Algorithms::Distance(begin, end), alloc)
        {
            *Algorithms::UninitializedCopy(begin, end, m_Data.Pointer) = T();
            m_Data.Size = Algorithms::Distance(begin, end);
        }

        template<ForwardIterator It>
        inline BasicString(It begin, It end, Alloc&& alloc = Alloc())
            : BasicString(Algorithms::Distance(begin, end), Move(alloc))
        {
            *Algorithms::UninitializedCopy(begin, end, m_Data.Pointer) = T();
            m_Data.Size = Algorithms::Distance(begin, end);
        }

        inline BasicString(const BasicString& str)
            : BasicString(str.Raw(), str.Size(), str.GetAllocator())
        {
        }

        inline BasicString(BasicString&& str)
        {
            m_Data = Move(str.m_Data);
            m_Allocator = Move(str.GetAllocator());

            str.m_Data.InitializeSSO();
        }

        inline BasicString(std::initializer_list<T> ilist, const Alloc& alloc)
            : BasicString(ilist.begin(), ilist.end(), alloc)
        {
        }

        inline BasicString(std::initializer_list<T> ilist, Alloc&& alloc = Alloc())
            : BasicString(ilist.begin(), ilist.end(), Move(alloc))
        {
        }

        inline BasicString(const ViewType& strv, const Alloc& alloc)
            : BasicString(strv.Data(), strv.Size(), alloc)
        {
        }

        inline BasicString(const ViewType& strv, Alloc&& alloc = Alloc())
            : BasicString(strv.Data(), strv.Size(), Move(alloc))
        {
        }

        inline ~BasicString() { Clear(); }

    public:
        inline BasicString& operator=(const BasicString& str)
        {
            if (this == &str) return *this;

            if ((m_Allocator != str.GetAllocator()) && !IsLocal())
                Clear();

            m_Allocator = str.GetAllocator();
            RangeAssign(str.GetBegin(), str.GetEnd());

            return *this;
        }

        inline BasicString& operator=(BasicString&& str)
        {
            if (this == &str) return *this;
            BasicString(Move(str)).Swap(*this);

            return *this;
        }

        inline BasicString& operator=(const T* str)
        {
            RangeAssign(str, str + ThisCharTraits::Length(str));
            return *this;
        }

        inline BasicString& operator=(T ch)
        {
            RangeAssign(&ch, &ch + 1);
            return *this;
        }

        inline BasicString& operator=(std::initializer_list<T> ilist)
        {
            RangeAssign(ilist.begin(), ilist.end());
            return *this;
        }

        inline BasicString& operator=(const ViewType& str)
        {
            RangeAssign(str.GetBegin(), str.GetEnd());
            return *this;
        }

        BasicString& operator=(std::nullptr_t) = delete;

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

        operator ViewType() const { return ViewType(Data(), Size()); }

        inline BasicString& operator+=(const BasicString& str)         { Append(str); return *this; }
        inline BasicString& operator+=(T ch)                           { Append(1, ch); return *this; }
        inline BasicString& operator+=(const T* str)                   { Append(str); return *this; }
        inline BasicString& operator+=(std::initializer_list<T> ilist) { Append(ilist); return *this; }
        inline BasicString& operator+=(const ViewType& strv)           { Append(strv); return *this; }

        inline BasicString operator+(const BasicString& str)         { BasicString ret = *this; ret += str; return ret; }
        inline BasicString operator+(T ch)                           { BasicString ret = *this; ret += ch; return ret; }
        inline BasicString operator+(const T* str)                   { BasicString ret = *this; ret += str; return ret; }
        inline BasicString operator+(std::initializer_list<T> ilist) { BasicString ret = *this; ret += ilist; return ret; }
        inline BasicString operator+(const ViewType& strv)           { BasicString ret = *this; ret += strv; return ret; }

    public:
        [[nodiscard]] inline T& Front()
        {
            if (IsEmpty()) throw OutOfRangeException();
            return *Data();
        }

        [[nodiscard]] inline const T& Front() const
        {
            if (IsEmpty()) throw OutOfRangeException();
            return *Data();
        }

        [[nodiscard]] inline T& Back()
        {
            if (IsEmpty()) throw OutOfRangeException();
            return Data()[Size() - 1];
        }

        [[nodiscard]] inline const T& Back() const
        {
            if (IsEmpty()) throw OutOfRangeException();
            return Data()[Size() - 1];
        }

        [[nodiscard]] inline T* Data()             { return m_Data.Pointer; }
        [[nodiscard]] inline const T* Data() const { return m_Data.Pointer; }

        [[nodiscard]] inline const T* Raw() const { return Data(); }

    public:
        [[nodiscard]] inline Usize Capacity() const
        {
            return IsLocal() ? StringData::SmallBufferSize : m_Data.Shared.Capacity;
        }

        [[nodiscard]] inline Usize Size() const   { return m_Data.Size; }
        [[nodiscard]] inline bool IsLocal() const { return m_Data.IsLocal(); }

        [[nodiscard]] inline Alloc& GetAllocator()             { return m_Allocator; }
        [[nodiscard]] inline const Alloc& GetAllocator() const { return m_Allocator; }

        [[nodiscard]] inline bool IsEmpty() const { return (Size() == 0); }

    public:
        [[nodiscard]] inline Iterator GetBegin()            { return Data(); }
        [[nodiscard]] inline ConstIterator GetBegin() const { return Data(); }

        [[nodiscard]] inline Iterator GetEnd()            { return Data() + Size(); }
        [[nodiscard]] inline ConstIterator GetEnd() const { return Data() + Size(); }

        [[nodiscard]] inline ReverseIterator GetReverseBegin()            { return ReverseIterator(GetEnd()); }
        [[nodiscard]] inline ReverseConstIterator GetReverseBegin() const { return ReverseConstIterator(GetEnd()); }

        [[nodiscard]] inline ReverseIterator GetReverseEnd()            { return ReverseIterator(GetBegin()); }
        [[nodiscard]] inline ReverseConstIterator GetReverseEnd() const { return ReverseConstIterator(GetBegin()); }

    public:
        inline void Reserve(Usize newCapacity)
        {
            if (newCapacity <= Capacity()) return;
            ReallocateGrowExact(newCapacity);
        }

        inline void ShrinkToFit()
        {
            if (Size() > StringData::SmallBufferSize)
                ReallocateGrowExact(Size());
            else
            {
                std::memcpy(m_Data.Shared.Buffer, m_Data.Pointer, (Size() + 1) * sizeof(T));
                m_Data.Pointer = m_Data.Shared.Buffer;
            }
        }

    public:
        inline void Clear()
        {
            if (IsLocal()) return;
            FreeAllocation(m_Data.Pointer);

            m_Data.InitializeSSO();
        }

        inline Iterator Insert(Iterator pos, const T* str)
        {
            return Insert(pos, str, ThisCharTraits::Length(str));
        }

        inline Iterator Insert(Iterator pos, const T* str, Usize size)
        {
            Iterator adjustedPos = ShiftEnd(pos, size);
            std::memcpy(adjustedPos, str, size * sizeof(T));

            return adjustedPos;
        }

        inline Iterator Insert(Iterator pos, const BasicString& str)
        {
            return Insert(pos, str.Raw(), str.Size());
        }

        inline Iterator Insert(Iterator pos, T ch) { return Insert(pos, 1, ch); }

        inline Iterator Insert(Iterator pos, Usize count, T ch)
        {
            Iterator adjustedPos = ShiftEnd(pos, count);
            Algorithms::UninitializedFillN(adjustedPos, count, ch);

            return adjustedPos;
        }

        inline Iterator Insert(Iterator pos, const ViewType& strv)
        {
            return Insert(pos, strv.Data(), strv.Size());
        }

        template<ForwardIterator It>
        inline Iterator Insert(Iterator pos, It begin, It end)
        {
            Iterator adjustedPos = ShiftEnd(pos, Algorithms::Distance(begin, end));
            Algorithms::UninitializedCopy(begin, end, adjustedPos);
        }

        inline Iterator Insert(Iterator pos, std::initializer_list<T> ilist)
        {
            return Insert(pos, ilist.begin(), ilist.end());
        }

        inline void Remove(Iterator pos) { Remove(pos, pos + 1); }
        inline void Remove(Iterator begin, Iterator end)
        {
            if ((begin < GetBegin()) || (begin >= GetEnd()) || (end < GetBegin()) || (end > GetEnd()))
                throw OutOfRangeException();

            Usize removedSize = static_cast<Usize>(Algorithms::Distance(begin, end));
            std::memmove(begin, end, (GetEnd() - end + 1) * sizeof(T));

            m_Data.Size -= removedSize;
        }

        inline void PushBack(T ch)
        {
            Usize newSize = Size() + 1;
            if (Capacity() < newSize)
                ReallocateGrowExact(newSize);

            ++m_Data.Size;
            Data()[Size() - 1] = ch;
            Data()[Size()] = T();
        }

        inline void PopBack()
        {
            if (IsEmpty())
                throw OutOfRangeException();

            --m_Data.Size;
            Data()[Size()] = T();
        }

        inline void Append(const BasicString& str) { Append(str.Raw(), str.Size()); }
        inline void Append(const T* str)           { Append(str, ThisCharTraits::Length(str)); }

        inline void Append(Usize count, T ch)
        {
            Usize newSize = Size() + count;
            if (Capacity() < newSize)
                ReallocateGrow(newSize);

            *Algorithms::UninitializedFillN(GetEnd(), count, ch) = T();
            m_Data.Size = newSize;
        }

        inline void Append(const T* str, Usize size)
        {
            Usize newSize = Size() + size;
            if (Capacity() < newSize)
                ReallocateGrow(newSize);

            std::memcpy(GetEnd(), str, size * sizeof(T));
            Data()[newSize] = T();

            m_Data.Size = newSize;
        }

        template<ForwardIterator It>
        inline void Append(It begin, It end)
        {
            Usize newSize = Size() + Algorithms::Distance(begin, end);
            if (Capacity() < newSize)
                ReallocateGrow(newSize);

            *Algorithms::UninitializedCopy(begin, end, GetEnd()) = T();
            m_Data.Size = newSize;
        }

        inline void Append(std::initializer_list<T> ilist) { Append(ilist.begin(), ilist.end()); }
        inline void Append(const ViewType& str)            { Append(str.Data(), str.Size()); }

        inline BasicString Substring(Usize startPos = 0, Usize count = 0) const
        {
            return BasicString(GetBegin() + startPos, count, GetAllocator());
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate range-based for loops.
        Iterator begin() { return GetBegin(); }
        ConstIterator begin() const { return GetBegin(); }

        Iterator end() { return GetEnd(); }
        ConstIterator end() const { return GetEnd(); }

    private:
        KITSUNE_FORCEINLINE Usize GetAdjustedCapacity(Usize cap)
        {
            // Too long, moved into seperate function.
            return static_cast<Usize>(static_cast<float>(cap) * s_AllocationFactor);
        }

        inline T* MakeAllocation(Usize count)
        {
            if (count == 0)
                return nullptr;

            return static_cast<T*>(m_Allocator.Allocate(count * sizeof(T), alignof(T)));
        }

        inline void FreeAllocation(T* ptr)
        {
            m_Allocator.Free(ptr);
        }

        inline void ReallocateGrow(Usize newCapacity)
        {
            ReallocateGrowExact(GetAdjustedCapacity(newCapacity));
        }

        inline void ReallocateGrowExact(Usize newCapacity)
        {
            T* ptr = MakeAllocation(newCapacity + 1);
            std::memcpy(ptr, Raw(), (Size() + 1) * sizeof(T));

            Usize size = Size();
            Clear();

            m_Data.Pointer = ptr;
            m_Data.Size = size;
            m_Data.Shared.Capacity = newCapacity;
        }

    private:
        template<ForwardIterator It>
        inline void RangeAssign(It begin, It end)
        {
            Usize size = static_cast<Usize>(Algorithms::Distance(begin, end));

            if (Capacity() < size)
                BasicString(size, Move(m_Allocator)).Swap(*this);

            *Algorithms::UninitializedCopy(begin, end, m_Data.Pointer) = T();
            m_Data.Size = size;
        }

        inline void InitializeCapacity(Usize size)
        {
            if (size <= StringData::SmallBufferSize)
                m_Data.InitializeSSO();
            else
            {
                Usize adjusted = GetAdjustedCapacity(size);

                m_Data.Pointer = MakeAllocation(adjusted + 1);
                m_Data.Shared.Capacity = adjusted;
            }
        }

        inline void Swap(BasicString& str)
        {
            Algorithms::Swap(m_Allocator, str.GetAllocator());
            m_Data.Swap(str.m_Data);
        }

        inline Iterator ShiftEnd(Iterator from, Usize offset)
        {
            if ((from < GetBegin()) || (from > GetEnd()))
                throw OutOfRangeException();

            Index index = from - GetBegin();
            Usize newSize = Size() + offset;

            if (Capacity() < newSize)
                ReallocateGrow(newSize);

            Iterator fromIt = GetBegin() + index;
            std::memmove(fromIt + offset, fromIt, sizeof(T) * (GetEnd() - fromIt + 1));

            m_Data.Size += offset;
            return fromIt;
        }

    private:
        static constexpr float s_AllocationFactor = 1.5f;

    private:
        class StringData
        {
        public:
            StringData() { InitializeSSO(); }
            StringData(StringData&& data)
                : Pointer(data.Pointer), Size(data.Size), Shared(data.Shared)
            {
                if (data.IsLocal())
                    Pointer = data.Shared.Buffer;

                data.InitializeSSO();
            }

            StringData& operator=(StringData&& data)
            {
                Size = data.Size;
                Shared = data.Shared;

                Pointer = data.IsLocal() ? Shared.Buffer : data.Pointer;

                data.InitializeSSO();
                return *this;
            }

        public:
            inline void InitializeSSO()
            {
                Pointer = Shared.Buffer;
                Pointer[0] = T();

                Size = 0;
            }

            inline bool IsLocal() const { return (Pointer == Shared.Buffer); }

            inline void Swap(StringData& data)
            {
                Algorithms::Swap(Pointer, data.Pointer);
                Algorithms::Swap(Size, data.Size);
                Algorithms::Swap(Shared, data.Shared);
            }

        public:
            static constexpr Usize SmallBufferSize = KITSUNE_MAX(2, 16 / sizeof(T)) - 1;

        public:
            T* Pointer;
            Usize Size;

            union
            {
                T Buffer[SmallBufferSize + 1];
                Usize Capacity;
            } Shared;
        };

        StringData m_Data;
        Alloc m_Allocator;
    };

    template<Character T, Allocator Alloc1, Allocator Alloc2>
    inline bool operator==(const BasicString<T, Alloc1>& str1, const BasicString<T, Alloc2>& str2)
    {
        return Algorithms::Equal(str1.GetBegin(), str1.GetEnd(), str2.GetBegin(), str2.GetEnd());
    }

    template<Character T, Allocator Alloc>
    inline bool operator==(const BasicString<T, Alloc>& str, const T* cstr)
    {
        Usize size = CharTraits<T>::Length(cstr);
        return Algorithms::Equal(str.GetBegin(), str.GetEnd(), cstr, cstr + size);
    }

    template<Character T, Allocator Alloc>
    inline bool operator==(const T* cstr, const BasicString<T, Alloc>& str)
    {
        return (str == cstr);
    }

    using String = BasicString<char>;
    using WideString = BasicString<wchar_t>;

    using U8String = BasicString<char8_t>;
    using U16String = BasicString<char16_t>;
    using U32String = BasicString<char32_t>;
}
