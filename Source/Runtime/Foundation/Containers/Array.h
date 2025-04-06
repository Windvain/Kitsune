#pragma once

#include <initializer_list>

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

#include "Foundation/Templates/Exchange.h"
#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/ReverseIterator.h"
#include "Foundation/Diagnostics/OutOfRangeException.h"

#include "Foundation/Algorithms/Swap.h"
#include "Foundation/Algorithms/Equal.h"
#include "Foundation/Algorithms/Destroy.h"

#include "Foundation/Algorithms/Distance.h"
#include "Foundation/Algorithms/Uninitialized.h"

namespace Kitsune
{
    template<typename T, Allocator Alloc = GlobalAllocator>
    class Array
    {
    public:
        using ValueType = T;
        using AllocatorType = Alloc;

        using Iterator = T*;
        using ConstIterator = const T*;

        using ReverseIterator = Kitsune::ReverseIterator<Iterator>;
        using ReverseConstIterator = Kitsune::ReverseIterator<ConstIterator>;

    public:
        inline Array() : m_Begin(nullptr), m_End(nullptr), m_StorageEnd(nullptr) { /* ... */ }

        inline explicit Array(const Alloc& alloc)
            : m_Begin(nullptr), m_End(nullptr), m_StorageEnd(nullptr),
              m_Allocator(alloc)
        {
        }

        inline explicit Array(Alloc&& alloc)
            : m_Begin(nullptr), m_End(nullptr), m_StorageEnd(nullptr),
             m_Allocator(Move(alloc))
        {
        }

        inline Array(Usize cap, const Alloc& alloc)
            : m_Allocator(alloc)
        {
            Usize adjusted = GetAdjustedCapacity(cap);

            m_Begin = MakeAllocation(adjusted);
            m_End = m_Begin;
            m_StorageEnd = m_Begin + adjusted;
        }

        inline explicit Array(Usize cap, Alloc&& alloc = Alloc())
            : m_Allocator(Move(alloc))
        {
            Usize adjusted = GetAdjustedCapacity(cap);

            m_Begin = MakeAllocation(adjusted);
            m_End = m_Begin;
            m_StorageEnd = m_Begin + adjusted;
        }

        inline Array(Usize count, const T& value, const Alloc& alloc)
            : Array(count, alloc)
        {
            m_End = Algorithms::UninitializedFillN(m_Begin, count, value);
        }

        inline Array(Usize count, const T& value, Alloc&& alloc = Alloc())
            : Array(count, Move(alloc))
        {
            m_End = Algorithms::UninitializedFillN(m_Begin, count, value);
        }

        template<ForwardIterator It>
        inline Array(It begin, It end, const Alloc& alloc)
            : Array(Algorithms::Distance(begin, end), alloc)
        {
            m_End = Algorithms::UninitializedCopy(begin, end, m_Begin);
        }

        template<ForwardIterator It>
        inline Array(It begin, It end, Alloc&& alloc = Alloc())
            : Array(Algorithms::Distance(begin, end), Move(alloc))
        {
            m_End = Algorithms::UninitializedCopy(begin, end, m_Begin);
        }

        inline Array(std::initializer_list<T> ilist, const Alloc& alloc)
            : Array(ilist.begin(), ilist.end(), alloc)
        {
        }

        inline Array(std::initializer_list<T> ilist, Alloc&& alloc = Alloc())
            : Array(ilist.begin(), ilist.end(), Move(alloc))
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

        inline ~Array() { Clear(); }

    public:
        inline Array& operator=(const Array& array)
        {
            if (this == &array) return *this;       // Ignore self-assigns.

            if (m_Allocator != array.GetAllocator())
                Clear();

            m_Allocator = array.GetAllocator();
            RangeAssign(array.GetBegin(), array.GetEnd());

            return *this;
        }

        inline Array& operator=(Array&& array)
        {
            if (this == &array) return *this;       // Ignore self-assigns.
            Array(Move(array)).Swap(*this);

            return *this;
        }

        inline Array& operator=(std::initializer_list<T> ilist)
        {
            RangeAssign(ilist.begin(), ilist.end());
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
            if (m_Begin == nullptr)
                throw OutOfRangeException();

            return *m_Begin;
        }

        [[nodiscard]]
        inline const T& Front() const
        {
            if (m_Begin == nullptr)
                throw OutOfRangeException();

            return *m_Begin;
        }

        [[nodiscard]] inline T& Back()
        {
            if (m_Begin == nullptr)
                throw OutOfRangeException();

            return *(m_End - 1);
        }

        [[nodiscard]] inline const T& Back() const
        {
            if (m_Begin == nullptr)
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
            if (newCapacity <= Capacity()) return;
            ReallocateGrowExact(newCapacity);
        }

        inline void ShrinkToFit() { ReallocateGrowExact(Size()); }

    public:
        void Swap(Array& array)
        {
            Algorithms::Swap(m_Begin, array.m_Begin);
            Algorithms::Swap(m_End, array.m_End);
            Algorithms::Swap(m_StorageEnd, array.m_StorageEnd);

            Algorithms::Swap(m_Allocator, array.m_Allocator);
        }

    public:
        inline void Clear()
        {
            if (m_Begin != nullptr)
            {
                Algorithms::Destroy(m_Begin, m_End);
                FreeAllocation(m_Begin);
            }

            m_Begin = m_End = m_StorageEnd = nullptr;
        }

        inline Iterator Insert(Iterator pos, const T& val) { return Emplace(pos, val); }
        inline Iterator Insert(Iterator pos, T&& val)      { return Emplace(pos, Move(val)); }

        inline Iterator Insert(Iterator pos, Usize count, const T& value)
        {
            Iterator adjustedPos = ShiftEnd(pos, count);
            Algorithms::UninitializedFillN(adjustedPos, count, value);

            return adjustedPos;
        }

        template<ForwardIterator It>
        inline Iterator Insert(Iterator pos, It begin, It end)
        {
            Iterator adjustedPos = ShiftEnd(pos, Algorithms::Distance(begin, end));
            Algorithms::UninitializedCopy(begin, end, adjustedPos);

            return adjustedPos;
        }

        inline Iterator Insert(Iterator pos, std::initializer_list<T> ilist)
        {
            return Insert(pos, ilist.begin(), ilist.end());
        }

        template<typename... Args>
        inline Iterator Emplace(Iterator pos, Args&&... args)
        {
            Iterator adjustedPos = ShiftEnd(pos, 1);
            Memory::ConstructAt(adjustedPos, Forward<Args>(args)...);

            return adjustedPos;
        }

        inline void Remove(Iterator pos) { return Remove(pos, pos + 1); }
        inline void Remove(Iterator begin, Iterator end)
        {
            if ((begin < GetBegin()) || (begin >= GetEnd()) || (end < GetBegin()) || (end > GetEnd()))
                throw OutOfRangeException();

            Usize removedSize = static_cast<Usize>(Algorithms::Distance(begin, end));
            Algorithms::Destroy(begin, end);

            for (auto it = end; it != GetEnd(); ++it, ++begin)
            {
                Memory::ConstructAt(begin, Move(*it));
                Memory::DestroyAt(it);
            }

            m_End -= removedSize;
        }

        inline void PushBack(const T& val) { EmplaceBack(val); }
        inline void PushBack(T&& val)      { EmplaceBack(Move(val)); }

        template<typename... Args>
        inline T& EmplaceBack(Args&&... args)
        {
            Usize newSize = Size() + 1;
            if (newSize > Capacity())
                ReallocateGrow(newSize);

            Memory::ConstructAt(m_End, Forward<Args>(args)...);
            return *(m_End++);
        }

        inline void PopBack()
        {
            if (IsEmpty()) throw OutOfRangeException();
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

        KITSUNE_FORCEINLINE void FreeAllocation(T* ptr)
        {
            m_Allocator.Free(ptr);
        }

        inline void ReallocateGrow(Usize newCapacity)
        {
            ReallocateGrowExact(GetAdjustedCapacity(newCapacity));
        }

        inline void ReallocateGrowExact(Usize newCapacity)
        {
            T* ptr = static_cast<T*>(m_Allocator.Allocate(newCapacity * sizeof(T), alignof(T)));
            T* end = Algorithms::UninitializedMove(m_Begin, m_End, ptr);

            Algorithms::Destroy(m_Begin, m_End);
            FreeAllocation(m_Begin);

            m_Begin = ptr;
            m_End = end;
            m_StorageEnd = ptr + newCapacity;
        }

    private:
        template<ForwardIterator It>
        void RangeAssign(It begin, It end)
        {
            Usize size = static_cast<Usize>(Algorithms::Distance(begin, end));

            if (Capacity() >= size)
                Algorithms::Destroy(m_Begin, m_End);
            else
                Array(size, Move(m_Allocator)).Swap(*this);

            m_End = Algorithms::UninitializedCopy(begin, end, m_Begin);
        }

        Iterator ShiftEnd(Iterator from, Usize offset)
        {
            if ((from < GetBegin()) || (from > GetEnd()))
                throw OutOfRangeException();

            Index index = from - GetBegin();
            Index reverseIndex = (Size() - index - 1);

            Usize newSize = Size() + offset;
            if (Capacity() < newSize)
                ReallocateGrow(newSize);

            for (auto it = GetReverseBegin(); it <= GetReverseBegin() + reverseIndex; ++it)
            {
                Memory::ConstructAt(AddressOf(*(it - offset)), Move(*it));
                Memory::DestroyAt(AddressOf(*it));
            }

            m_End += offset;
            return GetBegin() + index;
        }

    private:
        static constexpr float s_AllocationFactor = 1.5f;

    private:
        T *m_Begin, *m_End, *m_StorageEnd;
        KITSUNE_MAYBE_OVERLAPPING Alloc m_Allocator;
    };

    template<typename T, Allocator TAlloc, typename U, Allocator UAlloc>
    bool operator==(const Array<T, TAlloc>& arr1, const Array<U, UAlloc>& arr2)
        requires requires (T val1, U val2) { val1 == val2; }
    {
        return Algorithms::Equal(arr1.GetBegin(), arr1.GetEnd(), arr2.GetBegin(), arr2.GetEnd());
    }

    namespace Algorithms
    {
        template<typename T, Allocator Alloc>
        void Swap(Array<T, Alloc>& arr1, Array<T, Alloc>& arr2)
        {
            arr1.Swap(arr2);
        }
    }
}
