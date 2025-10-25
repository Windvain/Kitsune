#pragma once

#include "Foundation/Templates/Swap.h"
#include "Foundation/Concepts/Swappable.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It1, ForwardIterator It2>
        requires SwappableWith<typename IteratorTraits<It1>::ValueType,
                               typename IteratorTraits<It2>::ValueType>
    inline It2 Swap(It1 begin, It1 end, It2 outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            Kitsune::Swap(*begin, *outBegin);

        return outBegin;
    }
}
