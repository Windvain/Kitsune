#pragma once

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Kitsune::Algorithms
{
    // Returns the amount of increments needed for `begin` to reach `end`.
    template<RandomAccessIterator Iter>
    [[nodiscard]]
    inline typename IteratorTraits<Iter>::DifferenceType Distance(Iter begin, Iter end)
    {
        return (end - begin);
    }

    // Returns the amount of increments needed for `begin` to reach `end`.
    template<ForwardIterator Iter>
    [[nodiscard]]
    inline typename IteratorTraits<Iter>::DifferenceType Distance(Iter begin, Iter end)
    {
        typename IteratorTraits<Iter>::DifferenceType dist = 0;
        for (; begin != end; ++begin, ++dist);

        return dist;
    }
}
