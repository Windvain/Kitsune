#pragma once

#include "Foundation/Algorithms/Swap.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<RandomAccessIterator It>
    void Reverse(It begin, It end)
    {
        for (--end; end > begin; ++begin, --end)
            IteratorSwap(begin, end);
    }

    template<BidirectionalIterator It>
    void Reverse(It begin, It end)
    {
        for (; (begin != end) && (begin != --end); ++begin)
            IteratorSwap(begin, end);
    }
}
