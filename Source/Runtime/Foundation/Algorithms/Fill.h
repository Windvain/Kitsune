#pragma once

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename T>
    void Fill(It begin, It end, const T& val)
    {
        for (; begin != end; ++begin)
            *begin = val;
    }

    template<ForwardIterator It, typename Sz, typename T>
    It FillN(It begin, Sz n, const T& val)
    {
        for (; n > 0; ++begin, --n)
            *begin = val;

        return begin;
    }
}
