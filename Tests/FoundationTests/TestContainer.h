#pragma once

#include "Foundation/Iterators/IteratorTraits.h"

namespace Testing
{
    template<class It>
    class TestContainer
    {
    public:
        using ValueType = typename Kitsune::IteratorTraits<It>::ValueType;

    public:
        TestContainer() = default;
        TestContainer(ValueType* begin, ValueType* end)
            : Begin(begin), End(end)
        {
        }

    public:
        It Begin;
        It End;
    };
}
