#pragma once

#include <cstddef>
#include "TestIterators.h"

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Testing
{
    template<
        Iterator Iter, Iterator ConstIter,
        std::size_t ArraySize, bool IsOwning = true>
    class TestContainer
    {
    public:
        using ValueType = typename Kitsune::IteratorTraits<Iter>::ValueType;
        using StorageType = std::conditional_t<
            IsOwning,
            ValueType[ArraySize],
            ValueType*>;

        using Iterator = Iter;
        using ConstIterator = ConstIter;

    public:
        [[nodiscard]] inline Iterator GetBegin() { return Iterator(m_Array); }
        [[nodiscard]] inline Iterator GetEnd() { return Iterator(m_Array + ArraySize); }

        [[nodiscard]]
        inline ConstIterator GetBegin() const
        {
            return ConstIterator(m_Array);
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return ConstIterator(m_Array + ArraySize);
        }

    public:
        inline ValueType& operator[](std::size_t index)
        {
            return m_Array[index];
        }

        inline const ValueType& operator[](std::size_t index) const
        {
            return m_Array[index];
        }

    public:
        [[nodiscard]]
        inline Usize Size() const
        {
            return ArraySize;
        }

        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return (Size() == 0);
        }

    public:
        inline Iterator begin() { return GetBegin(); }
        inline ConstIterator begin() const { return GetBegin(); }

        inline Iterator end() { return GetEnd(); }
        inline ConstIterator end() const { return GetEnd(); }

    public:
        // Keep this member public, it is needed for aggregate initialization.
        StorageType m_Array;
    };

    template<typename T, std::size_t Size>
    using ForwardTestContainer = TestContainer<
        ForwardIterator<T>,
        ForwardIterator<const T>,
        Size>;

    template<typename T, std::size_t Size>
    using BidirectionalTestContainer = TestContainer<
        BidirectionalIterator<T>,
        BidirectionalIterator<const T>,
        Size>;

    template<typename T, std::size_t Size>
    using RandomAccessTestContainer = TestContainer<
        RandomAccessIterator<T>,
        RandomAccessIterator<const T>,
        Size>;

    template<typename T, std::size_t Size>
    using ForwardNonOwningTestContainer = TestContainer<
        ForwardIterator<T>,
        ForwardIterator<const T>,
        Size,
        false>;

    template<typename T, std::size_t Size>
    using BidirectionalNonOwningTestContainer = TestContainer<
        BidirectionalIterator<T>,
        BidirectionalIterator<const T>,
        Size,
        false>;

    template<typename T, std::size_t Size>
    using RandomAccessNonOwningTestContainer = TestContainer<
        RandomAccessIterator<T>,
        RandomAccessIterator<const T>,
        Size,
        false>;
}
