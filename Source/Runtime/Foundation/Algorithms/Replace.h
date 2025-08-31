#pragma once

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename T>
    inline void Replace(It begin, It end, const T& comp, const T& newValue)
    {
        const auto pred = [&comp](const IteratorTraits<It>::ValueType& elem) -> bool
        {
            return (elem == comp);
        };

        ReplaceIf(begin, end, pred, newValue);
    }

    template<ForwardIterator It, typename Size, typename T>
    inline void ReplaceN(It begin, Size n, const T& comp, const T& newValue)
    {
        for (; n > 0; ++begin, --n)
        {
            if (*begin == comp)
                *begin = newValue;
        }
    }

    template<ForwardIterator It,
             Invokable<typename IteratorTraits<It>::ValueType> Pred,
             typename T>
    inline void ReplaceIf(It begin, It end, Pred pred, const T& newValue)
    {
        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                *begin = newValue;
        }

    }
}
