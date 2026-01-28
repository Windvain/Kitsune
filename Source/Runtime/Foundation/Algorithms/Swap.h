#pragma once

#include "Foundation/Templates/Swap.h"
#include "Foundation/Concepts/Swappable.h"

namespace Kitsune::Algorithms
{
    // Swaps the elements in the range `[begin, end]` with the range `[begin2, -]`
    template<ForwardIterator Iter1, ForwardIterator Iter2>
        requires SwappableWith<typename IteratorTraits<Iter1>::ValueType,
                               typename IteratorTraits<Iter2>::ValueType>
    inline Iter2 Swap(Iter1 begin, Iter1 end, Iter2 begin2)
    {
        for (; begin != end; ++begin, ++begin2)
            Kitsune::Swap(*begin, *begin2);

        return begin2;
    }
}
