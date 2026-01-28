#pragma once

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    // Copies the contents in the range `[begin, end]` into `outBegin`, and returns an output
    // iterator pointing to the element one past the last copied element.
    template<ForwardIterator Iter,
             OutputIterator<typename IteratorTraits<Iter>::ValueType> OutIter>
    inline OutIter Copy(Iter begin, Iter end, OutIter outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            *outBegin = *begin;

        return outBegin;
    }

    // Copies the contents in the range `[begin, begin + n]` into `outBegin`, and returns an output
    // iterator pointing to the element one past the last copied element.
    template<ForwardIterator Iter, typename Size,
             OutputIterator<typename IteratorTraits<Iter>::ValueType> OutIter>
    inline OutIter CopyN(Iter begin, Size n, OutIter outBegin)
    {
        for (; n > 0; ++begin, ++outBegin, --n)
            *outBegin = *begin;

        return outBegin;
    }

    // Copies the contents in the range `[begin, end]` into `outBegin` which satisfy the
    // predicate `pred`, then returns an output iterator pointing to the element one
    // past the last copied element.
    template<ForwardIterator Iter,
             OutputIterator<typename IteratorTraits<Iter>::ValueType> OutIter,
             Invokable<typename IteratorTraits<Iter>::ValueType&> Pred>
    inline OutIter CopyIf(Iter begin, Iter end, OutIter outBegin, Pred pred)
    {
        for (; begin != end; ++begin)
        {
            if (pred(*begin))
            {
                *outBegin = *begin;
                ++outBegin;
            }
        }

        return outBegin;
    }

    // Copies the contents in the range `[begin, end]` into `outEnd` backwards, and returns
    // an output iterator pointing to the element one before the last copied element.
    template<BidirectionalIterator Iter, BidirectionalIterator OutIter>
    inline OutIter CopyBackwards(Iter begin, Iter end, OutIter outEnd)
    {
        while (begin != end)
        {
            --end; --outEnd;
            *outEnd = *end;
        }

        return outEnd;
    }
}
