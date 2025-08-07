#pragma once

#include <cstdint>
#include <algorithm>
#include <initializer_list>

#include "IteratorWrappers.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Testing
{
    template<typename Iter, std::size_t S>
    class TestContainer
    {
    public:
        using ValueType = typename Kitsune::IteratorTraits<Iter>::ValueType;

    public:
        TestContainer() = default;
        TestContainer(std::initializer_list<ValueType> ilist)
        {
            if (ilist.size() != S)
                throw;

            std::copy(ilist.begin(), ilist.end(), m_Array);
        }

        Iter GetBegin() { return Iter(m_Array); }
        Iter GetEnd() { return Iter(m_Array + S); }

    private:
        ValueType m_Array[S];
    };

    template<typename T, std::size_t S>
    using ForwardTestContainer = TestContainer<ForwardIteratorWrapper<T>, S>;

    template<typename T, std::size_t S>
    using BidirTestContainer = TestContainer<BidirIteratorWrapper<T>, S>;

    template<typename T, std::size_t S>
    using RandomAccessTestContainer = TestContainer<RandomAccessIteratorWrapper<T>, S>;
}
