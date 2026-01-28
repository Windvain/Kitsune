#pragma once

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Iterators/Iterator.h"

#include "Foundation/Concepts/Comparable.h"

namespace Kitsune::Algorithms
{
    // Checks whether the two ranges `[begin1, end1]` and `[begin2, -]` contain the same elements.
    // This function does not check the second range's size.
    template<ForwardIterator Iter1, ForwardIterator Iter2>
    [[nodiscard]]
    inline bool Equal(Iter1 begin1, Iter1 end1, Iter2 begin2)
        requires Equatable<typename IteratorTraits<Iter1>::ValueType,
                           typename IteratorTraits<Iter2>::ValueType>
    {
        using ValueType1 = IteratorTraits<Iter1>::ValueType;
        using ValueType2 = IteratorTraits<Iter2>::ValueType;

        return Equal(begin1, end1, begin2,
            [](const ValueType1& val1, const ValueType2& val2)
            {
                return (val1 == val2);
            });
    }

    // Checks whether the two ranges `[begin1, end1]` and `[begin2, -]` are both equal based on
    // the predicate `pred`. This function does not check the second range's size.
    template<ForwardIterator Iter1, ForwardIterator Iter2,
             Invokable<typename IteratorTraits<Iter1>::ValueType&,
                       typename IteratorTraits<Iter2>::ValueType&> Pred>
    [[nodiscard]]
    inline bool Equal(Iter1 begin1, Iter1 end1, Iter2 begin2, Pred pred)
    {
        for (; begin1 != end1; ++begin1, ++begin2)
        {
            if (!pred(*begin1, *begin2))
                return false;
        }

        return true;
    }

    // Checks whether the two ranges `[begin1, end1]` and `[begin2, end2]` contain the same
    // elements. This function does not check the second range's size.
    template<ForwardIterator Iter1, ForwardIterator Iter2>
    [[nodiscard]]
    inline bool Equal(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2)
        requires Equatable<typename IteratorTraits<Iter1>::ValueType,
                           typename IteratorTraits<Iter2>::ValueType>
    {
        using ValueType1 = IteratorTraits<Iter1>::ValueType;
        using ValueType2 = IteratorTraits<Iter2>::ValueType;

        return Equal(begin1, end1, begin2, end2,
            [](const ValueType1& val1, const ValueType2& val2)
            {
                return (val1 == val2);
            });
    }

    // Checks whether the two ranges `[begin1, end1]` and `[begin2, end2]` are both equal based on
    // the predicate `pred`. This function does not check the second range's size.
    template<ForwardIterator Iter1, ForwardIterator Iter2,
             Invokable<typename IteratorTraits<Iter1>::ValueType&,
                       typename IteratorTraits<Iter2>::ValueType&> Pred>
    [[nodiscard]]
    inline bool Equal(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2, Pred pred)
        requires RandomAccessIterator<Iter1> && RandomAccessIterator<Iter2>
    {
        if (end1 - begin1 != end2 - begin2)
            return false;

        for (; (begin1 != end1) && (begin2 != end2); ++begin1, ++begin2)
        {
            if (!pred(*begin1, *begin2))
                return false;
        }

        return true;
    }

    // Checks whether the two ranges `[begin1, end1]` and `[begin2, end2]` are both equal based on
    // the predicate `pred`. This function does not check the second range's size.
    template<ForwardIterator Iter1, ForwardIterator Iter2,
             Invokable<typename IteratorTraits<Iter1>::ValueType&,
                       typename IteratorTraits<Iter2>::ValueType&> Pred>
    [[nodiscard]]
    inline bool Equal(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2, Pred pred)
    {
        for (; (begin1 != end1) && (begin2 != end2); ++begin1, ++begin2)
        {
            if (!pred(*begin1, *begin2))
                return false;
        }

        return (begin1 == end1) && (begin2 == end2);
    }
}
