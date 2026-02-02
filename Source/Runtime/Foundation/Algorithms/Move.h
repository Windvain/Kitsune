#pragma once

#include "Foundation/Templates/Move.h"

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Kitsune::Algorithms
{
    // Moves all of the elements in the range `[begin, end]` into `outBegin`.
    template<ForwardIterator Iter,
             OutputIterator<typename IteratorTraits<Iter>::ValueType> OutIter>
    inline OutIter Move(Iter begin, Iter end, OutIter outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            *outBegin = Kitsune::Move(*begin);

        return outBegin;
    }

    // Moves all of the elements in the range `[begin, begin + n]` into `outBegin`.
    template<ForwardIterator Iter, typename Size,
             OutputIterator<typename IteratorTraits<Iter>::ValueType> OutIter>
    inline OutIter MoveN(Iter begin, Size n, OutIter outBegin)
    {
        for (; n > 0; ++begin, ++outBegin, --n)
            *outBegin = Kitsune::Move(*begin);

        return outBegin;
    }

    // Moves all of the elements in the range `[begin, end]` backwards into
    // `outEnd`, then returns an iterator pointing one before the last moved element.
    template<BidirectionalIterator Iter, BidirectionalIterator OutIter>
    inline OutIter MoveBackwards(Iter begin, Iter end, OutIter outEnd)
    {
        while (begin != end)
        {
            --end; --outEnd;
            *outEnd = Kitsune::Move(*end);
        }

        return outEnd;
    }
}
