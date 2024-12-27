#pragma once

#include "Foundation/Templates/Move.h"

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Algorithms/Internal/AlgoConcepts.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, ForwardIterator OutIt>
        requires Internal::IsIterMoveAssignable<OutIt, It>
    OutIt Move(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            *outBegin = Kitsune::Move(*begin);

        return outBegin;
    }

    template<ForwardIterator It, typename Sz, ForwardIterator OutIt>
        requires Internal::IsIterMoveAssignable<OutIt, It>
    OutIt MoveN(It begin, Sz n, OutIt outBegin)
    {
        for (; n > 0; ++begin, ++outBegin, --n)
            *outBegin = Kitsune::Move(*begin);

        return outBegin;
    }
}
