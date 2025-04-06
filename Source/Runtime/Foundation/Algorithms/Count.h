#pragma once

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename T>
    [[nodiscard]]
    typename IteratorTraits<It>::DifferenceType Count(It begin, It end, const T& val)
    {
        using ValueType = IteratorTraits<It>::ValueType;
        return CountIf(begin, end, [&val](const ValueType& elem) -> bool
        {
            return (val == elem);
        });
    }

    template<ForwardIterator It, typename Pred>
    [[nodiscard]]
    typename IteratorTraits<It>::DifferenceType CountIf(It begin, It end, Pred pred)
    {
        using Diff = IteratorTraits<It>::DifferenceType;
        Diff count = Diff();

        for (; begin != end; ++begin)
            if (pred(*begin)) { ++count; }

        return count;
    }
}
