#pragma once

#include "Foundation/Algorithms/Find.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator Iter, typename T>
    inline bool Contains(Iter begin, Iter end, const T& value)
    {
        return (Algorithms::Find(begin, end, value) != end);
    }

    template<ForwardIterator Iter, ForwardIterator RngIter>
    inline bool Contains(Iter begin, Iter end, RngIter rangeBegin, RngIter rangeEnd)
    {
        return (Algorithms::Find(begin, end, rangeBegin, rangeEnd) != end);
    }
}
