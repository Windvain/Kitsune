#pragma once

#include "Foundation/Iterators/Iterator.h"

#include "Foundation/Concepts/Invocable.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune::Algorithms
{
    // Returns an iterator to the first occurrence of the value `value` in the range
    // `[begin, end]`, or `end` if no occurrence were found.
    template<ForwardIterator Iter, typename T>
        requires Equatable<typename IteratorTraits<Iter>::ValueType, T>
    [[nodiscard]]
    inline Iter Find(Iter begin, Iter end, const T& value)
    {
        using ValueType = IteratorTraits<Iter>::ValueType;
        return FindIf(begin, end, [&value](const ValueType& elem)
        {
            return (value == elem);
        });
    }

    // Returns an iterator to the first occurrence of the range `[findBegin, findEnd]`
    // in the range `[begin, end]`, or `end` if no occurrence were found.
    template<ForwardIterator Iter1, ForwardIterator Iter2>
        requires Equatable<typename IteratorTraits<Iter1>::ValueType,
                           typename IteratorTraits<Iter2>::ValueType>
    [[nodiscard]]
    inline Iter1 Find(Iter1 begin, Iter1 end, Iter2 findBegin, Iter2 findEnd)
    {
        // Thanks MSVC.
        while (true)
        {
            Iter1 iter = begin;
            for (Iter2 findIter = findBegin; /* ... */; ++iter, ++findIter)
            {
                if (findIter == findEnd)
                    return begin;
                else if (iter == end)
                    return iter;

                if (*iter != *findIter)
                    break;
            }

            ++begin;
        }
    }

    // Returns an iterator to the first occurrence of a value which satisfies the
    // predicate `pred` in the range `[begin, end]`, or `end` if no occurrence were found.
    template<ForwardIterator Iter,
             Invocable<typename IteratorTraits<Iter>::ValueType> Pred>
    [[nodiscard]]
    inline Iter FindIf(Iter begin, Iter end, Pred pred)
    {
        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                return begin;
        }

        return begin;
    }

    // Returns an iterator to the last occurrence of the value `value` in the range
    // `[begin, end]`, or `end` if no occurrence were found.
    template<ForwardIterator Iter, typename T>
        requires Equatable<typename IteratorTraits<Iter>::ValueType, T>
    [[nodiscard]] inline Iter FindLast(Iter begin, Iter end, const T& value)
    {
        using ValueType = IteratorTraits<Iter>::ValueType;
        return FindLastIf(begin, end, [&value](const ValueType& elem) -> bool
        {
            return (value == elem);
        });
    }

    // Returns an iterator to the last occurrence of a value which satisfies the
    // predicate `pred` in the range `[begin, end]`, or `end` if no occurrence were found.
    template<ForwardIterator Iter,
             Invocable<typename IteratorTraits<Iter>::ValueType> Pred>
    [[nodiscard]] inline Iter FindLastIf(Iter begin, Iter end, Pred pred)
    {
        Iter last = end;
        for (; begin != end; ++begin)
        {
            if (pred(*begin))
                last = begin;
        }

        return last;
    }
}
