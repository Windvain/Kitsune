#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    // Gives a uniform interface to the properties of an iterator.
    // This class exists because a pointer is also an iterator.
    template<typename Iter>
    class IteratorTraits
    {
    public:
        using ValueType = typename Iter::ValueType;
        using DifferenceType = typename Iter::DifferenceType;
    };

    template<typename T>
    class IteratorTraits<T*>
    {
    public:
        using ValueType = T;
        using DifferenceType = Ptrdiff;
    };
}
