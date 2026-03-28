#pragma once

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune::Algorithms
{
    // Returns true if the range `[begin, end]` contains an element of the
    // specified value.
    template<ForwardIterator Iter, typename T>
        requires Equatable<typename IteratorTraits<Iter>::ValueType, T>
    inline bool Contains(Iter begin, Iter end, const T& value)
    {
        return (Algorithms::Find(begin, end, value) != end);
    }

    // Returns true if the range `[begin, end]` contains the range
    // `[rangeBegin, rangeEnd]`.
    template<ForwardIterator Iter, ForwardIterator RngIter>
        requires Equatable<typename IteratorTraits<Iter>::ValueType,
                           typename IteratorTraits<RngIter>::ValueType>
    inline bool Contains(Iter begin, Iter end, RngIter rangeBegin, RngIter rangeEnd)
    {
        return (Algorithms::Find(begin, end, rangeBegin, rangeEnd) != end);
    }

    // Returns true if the range `[begin, end]` contains an element that
    // satisfies the predicate `pred`.
    template<ForwardIterator Iter,
             Invocable<typename IteratorTraits<Iter>::ValueType> Pred>
    inline bool ContainsIf(Iter begin, Iter end, Pred pred)
    {
        return (Algorithms::FindIf(begin, end, pred) != end);
    }
}
