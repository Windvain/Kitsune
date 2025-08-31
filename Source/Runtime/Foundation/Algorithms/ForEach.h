#pragma once

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename Fn>
    inline void ForEach(It begin, It end, Fn fn)
    {
        for (; begin != end; ++begin)
            fn(*begin);
    }

    template<ForwardIterator It, typename Size, typename Fn>
    inline void ForEachN(It begin, Size n, Fn fn)
    {
        for (; n > 0; ++begin, --n)
            fn(*begin);
    }

    template<ForwardIterator It, typename Fn,
             Invokable<typename IteratorTraits<It>::ValueType> Pred>
    inline void ForEachIf(It begin, It end, Pred pred, Fn fn)
    {
        for (; begin != end; ++begin)
            if (pred(*begin)) { fn(*begin); }
    }
}
