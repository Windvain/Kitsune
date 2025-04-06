#pragma once

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename T>
    void Replace(It begin, It end, const T& comp, const T& newValue)
    {
        const auto pred = [&comp](const IteratorTraits<It>::ValueType& elem) -> bool
        {
            return (elem == comp);
        };

        ReplaceIf(begin, end, pred, newValue);
    }

    template<ForwardIterator It, typename Size, typename T>
    void ReplaceN(It begin, Size n, const T& comp, const T& newValue)
    {
        for (; n > 0; ++begin, --n)
        {
            if (*begin == comp)
                *begin = newValue;
        }
    }

    template<ForwardIterator It, typename Pred, typename T>
    void ReplaceIf(It begin, It end, Pred pred, const T& newValue)
    {
        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                *begin = newValue;
        }

    }
}
