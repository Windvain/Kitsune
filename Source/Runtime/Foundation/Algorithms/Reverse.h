#pragma once

#include "Foundation/Templates/Swap.h"
#include "Foundation/Concepts/Swappable.h"

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    // Reverses the elements in the range `[begin, end]`.
    template<RandomAccessIterator Iter>
        requires Swappable<typename IteratorTraits<Iter>::ValueType>
    inline void Reverse(Iter begin, Iter end)
    {
        for (--end; end > begin; ++begin, --end)
            IteratorSwap(begin, end);
    }

    // Reverses the elements in the range `[begin, end]`.
    template<BidirectionalIterator Iter>
        requires Swappable<typename IteratorTraits<Iter>::ValueType>
    inline void Reverse(Iter begin, Iter end)
    {
        for (; (begin != end) && (begin != --end); ++begin)
            IteratorSwap(begin, end);
    }
}
