#pragma once

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It,
             WritableIterator<typename IteratorTraits<It>::ValueType> OutIt>
    OutIt Copy(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            *outBegin = *begin;

        return outBegin;
    }

    template<ForwardIterator It, typename Sz,
             WritableIterator<typename IteratorTraits<It>::ValueType> OutIt>
    OutIt CopyN(It begin, Sz n, OutIt outBegin)
    {
        for (; n > 0; ++begin, ++outBegin, --n)
            *outBegin = *begin;

        return outBegin;
    }

    template<ForwardIterator It,
             WritableIterator<typename IteratorTraits<It>::ValueType> OutIt, typename Pred>
    OutIt CopyIf(It begin, It end, OutIt outBegin, Pred pred)
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

    template<BidirectionalIterator It, BidirectionalIterator OutIt>
    OutIt CopyBackwards(It begin, It end, OutIt outEnd)
    {
        while (begin != end)
        {
            --end; --outEnd;
            *outEnd = *end;
        }

        return outEnd;
    }
}
