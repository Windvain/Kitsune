#pragma once

#include "Foundation/Iterators/Iterator.h"

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune::Algorithms
{
    // Returns the amount of occurrences of the value `value` in the
    // range `[begin, end]`.
    template<ForwardIterator Iter, typename T>
        requires Equatable<typename IteratorTraits<Iter>::ValueType, T>
    [[nodiscard]]
    inline typename IteratorTraits<Iter>::DifferenceType Count(
        Iter begin, Iter end, const T& value)
    {
        using ValueType = IteratorTraits<Iter>::ValueType;
        return CountIf(begin, end, [&value](const ValueType& elem) -> bool
        {
            return (value == elem);
        });
    }

    // Returns the amount of elements which satisfy `pred` in the range `[begin, end]`.
    template<ForwardIterator Iter,
             Invokable<typename IteratorTraits<Iter>::ValueType&> Pred>
    [[nodiscard]]
    inline typename IteratorTraits<Iter>::DifferenceType CountIf(
        Iter begin, Iter end, Pred pred)
    {
        using Diff = IteratorTraits<Iter>::DifferenceType;
        Diff count = Diff();

        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                ++count;
        }

        return count;
    }
}
