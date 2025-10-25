#pragma once

#include "Foundation/Templates/Swap.h"
#include "Foundation/Concepts/Swappable.h"

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<RandomAccessIterator It>
        requires Swappable<typename IteratorTraits<It>::ValueType>
    inline void Reverse(It begin, It end)
    {
        for (--end; end > begin; ++begin, --end)
            IteratorSwap(begin, end);
    }

    template<BidirectionalIterator It>
        requires Swappable<typename IteratorTraits<It>::ValueType>
    inline void Reverse(It begin, It end)
    {
        for (; (begin != end) && (begin != --end); ++begin)
            IteratorSwap(begin, end);
    }
}
