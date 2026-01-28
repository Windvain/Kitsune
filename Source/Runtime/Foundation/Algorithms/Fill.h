#pragma once

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    // Fills the range `[begin, end]` with the value `value`.
    template<ForwardIterator Iter, typename T>
    inline void Fill(Iter begin, Iter end, const T& value)
    {
        for (; begin != end; ++begin)
            *begin = value;
    }

    // Fills the range `[begin, begin + n]` with the value `value`.
    template<ForwardIterator Iter, typename Size, typename T>
    inline Iter FillN(Iter begin, Size n, const T& value)
    {
        for (; n > 0; ++begin, --n)
            *begin = value;

        return begin;
    }
}
