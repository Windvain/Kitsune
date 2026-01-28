#pragma once

#include <initializer_list>
#include "Foundation/Concepts/Comparable.h"

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Templates/Swap.h"
#include "Foundation/Templates/Exchange.h"

#include "Foundation/Algorithms/Equal.h"
#include "Foundation/Algorithms/Destroy.h"

#include "Foundation/Algorithms/Distance.h"
#include "Foundation/Algorithms/Uninitialized.h"

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/ReverseIterator.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

namespace Kitsune
{
    // A container that stores a contiguous array of elements with type `T`.
    template<typename T, Allocator Alloc = GlobalAllocator>
    class Array
    {
    private:
        // std::initializer_list is usually implemented as a pair of pointers or as a pointer and size pair.
        // This assertion should not fail with the commonly used compilers.
        static_assert(ForwardIterator<typename std::initializer_list<T>::iterator>,
                      "std::initializer_list iterators do not satisfy ForwardIterator.");

        static_assert(ForwardIterator<typename std::initializer_list<T>::const_iterator>,
                      "std::initializer_list iterators do not satisfy ForwardIterator.");

    public:
        using ValueType = T;
        using AllocatorType = Alloc;

        using Iterator = T*;
        using ConstIterator = const T*;

        using ReverseIterator = Kitsune::ReverseIterator<Iterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline Array()
            : m_Begin(nullptr), m_End(nullptr), m_StorageEnd(nullptr),
              m_Allocator()
        {
        }

        inline explicit Array(const Alloc& alloc)
            : m_Begin(nullptr), m_End(nullptr), m_StorageEnd(nullptr),
              m_Allocator(alloc)
        {
        }

        inline Array(Usize capacity, const Alloc& alloc = Alloc())
            : Array(alloc)
        {
            if (capacity == 0)
                return;

            Usize adjustedCapacity = s_AllocationFactor * capacity;
            void* data = m_Allocator.Allocate(adjustedCapacity * sizeof(T), alignof(T));

            m_Begin = m_End = static_cast<T*>(data);
            m_StorageEnd = m_Begin + capacity;
        }

        inline Array(Usize count, const T& value, const Alloc& alloc = Alloc())
            : Array(count, alloc)
        {
            m_End = Algorithms::UninitializedFillN(m_Begin, count, value);
        }


        template<ForwardIterator It>
        inline Array(It begin, It end, const Alloc& alloc = Alloc())
            : Array(Algorithms::Distance(begin, end), alloc)
        {
            m_End = Algorithms::UninitializedCopy(begin, end, m_Begin);
        }

        inline Array(std::initializer_list<T> ilist, const Alloc& alloc = Alloc())
            : Array(ilist.begin(), ilist.end(), alloc)
        {
        }

        inline Array(const Array& array)
            : Array(array.GetBegin(), array.GetEnd(), array.GetAllocator())
        {
        }

        inline Array(Array&& array)
            : m_Begin(Exchange(array.m_Begin, nullptr)),
              m_End(Exchange(array.m_End, nullptr)),
              m_StorageEnd(Exchange(array.m_StorageEnd, nullptr)),
              m_Allocator(Move(array.GetAllocator()))
        {
        }

        inline ~Array()
        {
            if (m_Begin == nullptr)
                return;

            Algorithms::Destroy(m_Begin, m_End);
            m_Allocator.Free(m_Begin, Capacity());
        }

    public:
        inline Array& operator=(const Array& array)
        {
            if (this == &array)
                return *this;

            if (m_Allocator != array.GetAllocator())
                Array().Swap(*this);

            m_Allocator = array.GetAllocator();
            Assign(array.GetBegin(), array.GetEnd());

            return *this;
        }

        inline Array& operator=(Array&& array)
        {
            if (this == &array)
                return *this;

            Array(Move(array)).Swap(*this);
            return *this;
        }

        inline Array& operator=(std::initializer_list<T> ilist)
        {
            Assign(ilist.begin(), ilist.end());
            return *this;
        }

    public:
        inline T& operator[](Index index)
        {
            if (index >= Size())
                throw OutOfRangeException();

            return m_Begin[index];
        }

        inline const T& operator[](Index index) const
        {
            if (index >= Size())
                throw OutOfRangeException();

            return m_Begin[index];
        }

    public:
        [[nodiscard]]
        inline T& Front()
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return *m_Begin;
        }

        [[nodiscard]]
        inline const T& Front() const
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return *m_Begin;
        }

        [[nodiscard]] inline T& Back()
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return *(m_End - 1);
        }

        [[nodiscard]] inline const T& Back() const
        {
            if (IsEmpty())
                throw OutOfRangeException();

            return *(m_End - 1);
        }

        [[nodiscard]] inline T* Data()             { return m_Begin; }
        [[nodiscard]] inline const T* Data() const { return m_Begin; }

        [[nodiscard]] inline Alloc& GetAllocator()             { return m_Allocator; }
        [[nodiscard]] inline const Alloc& GetAllocator() const { return m_Allocator; }

    public:
        [[nodiscard]] inline Usize Size() const
        {
            return static_cast<Usize>(m_End - m_Begin);
        }

        [[nodiscard]] inline Usize Capacity() const
        {
            return static_cast<Usize>(m_StorageEnd - m_Begin);
        }

        [[nodiscard]]
        inline bool IsEmpty() const { return (m_Begin == m_End); }

    public:
        [[nodiscard]] inline Iterator GetBegin()            { return m_Begin; }
        [[nodiscard]] inline ConstIterator GetBegin() const { return m_Begin; }

        [[nodiscard]] inline Iterator GetEnd()            { return m_End; }
        [[nodiscard]] inline ConstIterator GetEnd() const { return m_End; }

        [[nodiscard]] inline ReverseIterator GetReverseBegin()            { return ReverseIterator(m_End); }
        [[nodiscard]] inline ReverseConstIterator GetReverseBegin() const { return ReverseIterator(m_End); }

        [[nodiscard]] inline ReverseIterator GetReverseEnd()            { return ReverseIterator(m_Begin); }
        [[nodiscard]] inline ReverseConstIterator GetReverseEnd() const { return ReverseIterator(m_Begin); }

    public:
        inline void Reserve(Usize newCapacity)
        {
            if (newCapacity <= Capacity())
                return;

            ReallocateExact(newCapacity);
        }

        inline void ShrinkToFit()
        {
            ReallocateExact(Size());
        }

    public:
        inline void Swap(Array& array)
        {
            Kitsune::Swap(m_Begin, array.m_Begin);
            Kitsune::Swap(m_End, array.m_End);
            Kitsune::Swap(m_StorageEnd, array.m_StorageEnd);

            Kitsune::Swap(m_Allocator, array.m_Allocator);
        }

    public:
        inline void Clear()
        {
            Algorithms::Destroy(m_Begin, m_End);
            m_End = m_Begin;
        }

        template<ForwardIterator It>
        inline void Assign(It begin, It end)
        {
            Usize size = static_cast<Usize>(Algorithms::Distance(begin, end));

            if (Capacity() >= size)
                Algorithms::Destroy(m_Begin, m_End);
            else
                Array(size, Move(m_Allocator)).Swap(*this);

            m_End = Algorithms::UninitializedCopy(begin, end, m_Begin);
        }

        inline void Assign(std::initializer_list<T> ilist)
        {
            Assign(ilist.begin(), ilist.end());
        }

        inline void Assign(Usize count, const T& value)
        {
            if (Capacity() >= count)
                Algorithms::Destroy(m_Begin, m_End);
            else
                Array(count, Move(m_Allocator)).Swap(*this);

            m_End = Algorithms::UninitializedFillN(m_Begin, count, value);
        }

    public:
        inline Iterator Insert(Iterator pos, const T& val)
        {
            return Emplace(pos, val);
        }

        inline Iterator Insert(Iterator pos, T&& val)
        {
            return Emplace(pos, Move(val));
        }

        inline Iterator Insert(Iterator pos, Usize count, const T& value)
        {
            for (Usize i = 0; i != count; ++i, ++pos)
                pos = Insert(pos, value);

            return pos - count;
        }

        template<ForwardIterator It>
        inline Iterator Insert(Iterator pos, It begin, It end)
        {
            typename IteratorTraits<It>::DifferenceType rangeLen = 0;
            for (; begin != end; ++begin, ++pos, ++rangeLen)
                pos = Insert(pos, *begin);

            return pos - rangeLen;
        }

        inline Iterator Insert(Iterator pos, std::initializer_list<T> ilist)
        {
            return Insert(pos, ilist.begin(), ilist.end());
        }

        template<typename... Args>
        inline Iterator Emplace(Iterator pos, Args&&... args)
        {
            if ((pos < GetBegin()) || (pos > GetEnd()))
                throw OutOfRangeException();

            Usize newSize = Size() + 1;
            if (Capacity() < newSize)
            {
                Index index = pos - GetBegin();
                Reallocate(newSize);

                pos = GetBegin() + index;
            }

            // Shift the elements of the range [pos, GetEnd()] starting from the end.
            auto sourceShift = GetReverseBegin();
            auto destShift = ReverseIterator(m_Begin + newSize);

            for (; sourceShift != ReverseIterator(pos); ++sourceShift, ++destShift)
            {
                Memory::ConstructAt(AddressOf(*destShift), Move(*sourceShift));
                Memory::DestroyAt(AddressOf(*sourceShift));
            }

            Memory::ConstructAt(AddressOf(*pos), Forward<Args>(args)...);

            ++m_End;
            return pos;
        }

        inline void Remove(Iterator pos)
        {
            return Remove(pos, pos + 1);
        }

        inline void Remove(Iterator begin, Iterator end)
        {
            if (begin == end)
                return;

            if ((begin < GetBegin()) || (begin >= GetEnd()) || (end < GetBegin()) || (end > GetEnd()))
                throw OutOfRangeException();

            Ptrdiff removedSize = end - begin;
            Algorithms::Destroy(begin, end);

            for (auto it = end; it != GetEnd(); ++it, ++begin)
            {
                Memory::ConstructAt(begin, Move(*it));
                Memory::DestroyAt(it);
            }

            m_End -= removedSize;
        }

        inline void RemoveUnsorted(Iterator it)
        {
            RemoveUnsorted(it, it + 1);
        }

        inline void RemoveUnsorted(Iterator begin, Iterator end)
        {
            if (begin == end)
                return;

            if ((begin < GetBegin()) || (begin >= GetEnd()) || (end < GetBegin()) || (end > GetEnd()))
                throw OutOfRangeException();

            Ptrdiff removedSize = end - begin;
            Algorithms::Destroy(begin, end);

            Algorithms::UninitializedMoveN(GetReverseBegin(), KITSUNE_MIN(removedSize, GetEnd() - end), begin);
            m_End -= removedSize;
        }

        inline void PushBack(const T& val)
        {
            EmplaceBack(val);
        }

        inline void PushBack(T&& val)
        {
            EmplaceBack(Move(val));
        }

        template<typename... Args>
        inline T& EmplaceBack(Args&&... args)
        {
            Usize newSize = Size() + 1;
            if (newSize > Capacity())
                Reallocate(newSize);

            Memory::ConstructAt(m_End, Forward<Args>(args)...);
            return *(m_End++);
        }

        inline void PopBack()
        {
            if (IsEmpty())
                throw OutOfRangeException();

            Memory::DestroyAt(--m_End);
        }

    public:
        // Should not be called by engine/client code.
        // Made public so that the compiler can generate code for range-based for loops.
        inline Iterator begin() { return GetBegin(); }
        inline ConstIterator begin() const { return GetBegin(); }

        inline Iterator end() { return GetEnd(); }
        inline ConstIterator end() const { return GetEnd(); }

    private:
        inline void ReallocateExact(Usize newCapacity)
        {
            T* pointer = static_cast<T*>(m_Allocator.Allocate(newCapacity * sizeof(T), alignof(T)));
            Usize moveCount = KITSUNE_MIN(newCapacity, Size());

            Algorithms::UninitializedMoveN(m_Begin, moveCount, pointer);
            Algorithms::Destroy(m_Begin, m_End);

            m_Allocator.Free(m_Begin, Capacity());

            m_Begin = pointer;
            m_End = m_Begin + moveCount;
            m_StorageEnd = m_Begin + newCapacity;
        }

        inline void Reallocate(Usize newCapacity)
        {
            Usize adjustedCapacity = s_AllocationFactor * newCapacity;
            ReallocateExact(adjustedCapacity);
        }

    private:
        static constexpr float s_AllocationFactor = 1.5f;

    private:
        T *m_Begin, *m_End, *m_StorageEnd;
        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };

    template<typename T, Allocator TAlloc, typename U, Allocator UAlloc>
        requires Equatable<T, U>
    inline bool operator==(const Array<T, TAlloc>& arr1, const Array<U, UAlloc>& arr2)
    {
        return Algorithms::Equal(arr1.GetBegin(), arr1.GetEnd(), arr2.GetBegin(), arr2.GetEnd());
    }
}
