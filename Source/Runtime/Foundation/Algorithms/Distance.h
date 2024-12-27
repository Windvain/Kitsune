#pragma once

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Kitsune::Algorithms
{
    template<RandomAccessIterator It>
    [[nodiscard]]
    typename IteratorTraits<It>::DifferenceType Distance(It begin, It end)
    {
        return (end - begin);
    }

    template<ForwardIterator It>
    [[nodiscard]]
    typename IteratorTraits<It>::DifferenceType Distance(It begin, It end)
    {
        typename IteratorTraits<It>::DifferenceType dist = 0;
        for (; begin != end; ++begin, ++dist);

        return dist;
    }
}
