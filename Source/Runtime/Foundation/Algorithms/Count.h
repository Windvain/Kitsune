#pragma once

#include "Foundation/Iterators/Iterator.h"

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename T>
        requires Equatable<typename IteratorTraits<It>::ValueType, T>
    [[nodiscard]]
    inline typename IteratorTraits<It>::DifferenceType Count(It begin, It end, const T& val)
    {
        using ValueType = IteratorTraits<It>::ValueType;
        return CountIf(begin, end, [&val](const ValueType& elem) -> bool
        {
            return (val == elem);
        });
    }

    template<ForwardIterator It, Invokable<typename IteratorTraits<It>::ValueType&> Pred>
    [[nodiscard]]
    inline typename IteratorTraits<It>::DifferenceType CountIf(It begin, It end, Pred pred)
    {
        using Diff = IteratorTraits<It>::DifferenceType;
        Diff count = Diff();

        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                ++count;
        }

        return count;
    }
}
