#pragma once

#include "Foundation/Iterators/IteratorTraits.h"
#include "Foundation/Iterators/Iterator.h"

#include "Foundation/Algorithms/Internal/AlgoConcepts.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename Fn>
        requires Internal::IsInvocableWithIterator<It, Fn>
    void ForEach(It begin, It end, Fn fn)
    {
        for (; begin != end; ++begin)
            fn(*begin);
    }

    template<ForwardIterator It, typename Fn, typename Pred>
        requires Internal::IsInvocableWithIterator<It, Fn> &&
                 Internal::IsPredicateSingular<It, Pred>
    void ForEachIf(It begin, It end, Pred pred, Fn fn)
    {
        for (; begin != end; ++begin)
            if (pred(*begin)) { fn(*begin); }
    }
}
