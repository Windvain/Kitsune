#pragma once

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    // Invokes the function `func` for every element in `[begin, end]`.
    template<ForwardIterator Iter,
             Invokable<typename IteratorTraits<Iter>::ValueType&> Func>
    inline void ForEach(Iter begin, Iter end, Func func)
    {
        for (; begin != end; ++begin)
            func(*begin);
    }

    // Invokes the function `func` for every element in `[begin, begin + n]`.
    template<ForwardIterator Iter, typename Size,
             Invokable<typename IteratorTraits<Iter>::ValueType&> Func>
    inline void ForEachN(Iter begin, Size n, Func func)
    {
        for (; n > 0; ++begin, --n)
            func(*begin);
    }

    // Invokes the function `func` for every element in `[begin, end]` which
    // satisfies the predicate `pred`.
    template<ForwardIterator Iter,
             Invokable<typename IteratorTraits<Iter>::ValueType&> Func,
             Invokable<typename IteratorTraits<Iter>::ValueType&> Pred>
    inline void ForEachIf(Iter begin, Iter end, Pred pred, Func func)
    {
        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                func(*begin);
        }
    }
}
