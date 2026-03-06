#pragma once

#include <cstddef>
#include "TestIterators.h"

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Testing
{
    template<Iterator Iter, std::size_t S>
    class TestContainer
    {
    public:
        using ValueType = typename Kitsune::IteratorTraits<Iter>::ValueType;

    public:
        Iter GetBegin() { return Iter(m_Array); }
        Iter GetEnd()   { return Iter(m_Array + S); }

    public:
        ValueType& operator[](std::size_t index)
        {
            return m_Array[index];
        }

        const ValueType& operator[](std::size_t index) const
        {
            return m_Array[index];
        }

    public:
        Iter begin() { return GetBegin(); }
        Iter end()   { return GetEnd(); }

    public:
        ValueType m_Array[S] = { /* ... */ };
    };

    template<typename T, std::size_t S>
    using ForwardTestContainer = TestContainer<ForwardIterator<T>, S>;

    template<typename T, std::size_t S>
    using BidirectionalTestContainer =
        TestContainer<BidirectionalIterator<T>, S>;

    template<typename T, std::size_t S>
    using RandomAccessTestContainer = TestContainer<RandomAccessIterator<T>, S>;
}
