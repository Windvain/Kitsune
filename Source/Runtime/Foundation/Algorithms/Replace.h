#pragma once

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    // Replaces all occurances of `comp` in the range `[begin, end]` with `newValue`.
    template<ForwardIterator Iter, typename T>
        requires Equatable<typename IteratorTraits<Iter>::ValueType, T>
    inline void Replace(Iter begin, Iter end, const T& comp, const T& newValue)
    {
        const auto pred = [&comp](const IteratorTraits<Iter>::ValueType& elem) -> bool
        {
            return (elem == comp);
        };

        ReplaceIf(begin, end, pred, newValue);
    }

    // Replaces all occurances of `comp` in the range `[begin, begin + n]`
    // with `newValue`.
    template<ForwardIterator Iter, typename Size, typename T>
        requires Equatable<typename IteratorTraits<Iter>::ValueType, T>
    inline void ReplaceN(Iter begin, Size n, const T& comp, const T& newValue)
    {
        for (; n > 0; ++begin, --n)
        {
            if (*begin == comp)
                *begin = newValue;
        }
    }

    // Replaces all elements in the range `[begin, begin + n]` which satisfy the
    // predicate `pred` with `newValue`.
    template<ForwardIterator Iter,
             Invokable<typename IteratorTraits<Iter>::ValueType&> Pred,
             typename T>
    inline void ReplaceIf(Iter begin, Iter end, Pred pred, const T& newValue)
    {
        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                *begin = newValue;
        }
    }
}
