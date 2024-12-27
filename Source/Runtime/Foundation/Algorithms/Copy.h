#pragma once

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Algorithms/Internal/AlgoConcepts.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, ForwardIterator OutIt>
        requires Internal::IsIterCopyAssignable<OutIt, It>
    OutIt Copy(It begin, It end, OutIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            *outBegin = *begin;

        return outBegin;
    }

    template<ForwardIterator It, typename Sz, ForwardIterator OutIt>
        requires Internal::IsIterCopyAssignable<OutIt, It>
    OutIt CopyN(It begin, Sz n, OutIt outBegin)
    {
        for (; n > 0; ++begin, ++outBegin, --n)
            *outBegin = *begin;

        return outBegin;
    }

    template<ForwardIterator It, ForwardIterator OutIt, typename Pred>
        requires Internal::IsIterCopyAssignable<OutIt, It> &&
                 Internal::IsPredicateSingular<It, Pred>
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
}
