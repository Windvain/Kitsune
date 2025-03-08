#pragma once

#include "Foundation/Templates/Move.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, ForwardIterator OutIt>
    OutIt Move(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            *outBegin = Kitsune::Move(*begin);

        return outBegin;
    }

    template<ForwardIterator It, typename Sz, ForwardIterator OutIt>
    OutIt MoveN(It begin, Sz n, OutIt outBegin)
    {
        for (; n > 0; ++begin, ++outBegin, --n)
            *outBegin = Kitsune::Move(*begin);

        return outBegin;
    }

    template<BidirectionalIterator It, BidirectionalIterator OutIt>
    OutIt MoveBackwards(It begin, It end, OutIt outEnd)
    {
        while (begin != end)
        {
            --end; --outEnd;
            *outEnd = Kitsune::Move(*end);
        }

        return outEnd;
    }
}
