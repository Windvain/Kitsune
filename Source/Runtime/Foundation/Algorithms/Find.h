#pragma once

#include "Foundation/Iterators/Iterator.h"

#include "Foundation/Concepts/Invokable.h"
#include "Foundation/Concepts/Comparable.h"

namespace Kitsune::Algorithms
{
    // Returns an iterator to the first occurance of the value `value` in the range
    // `[begin, end]`, or `end` if no occurences were found.
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

    // Returns an iterator to the first occurance of the range `[findBegin, findEnd]` in the
    // range `[begin, end]`, or `end` if no occurences were found.
    template<ForwardIterator Iter1, ForwardIterator Iter2>
        requires Equatable<typename IteratorTraits<Iter1>::ValueType,
                           typename IteratorTraits<Iter2>::ValueType>
    [[nodiscard]]
    inline Iter1 Find(Iter1 begin, Iter1 end, Iter2 findBegin, Iter2 findEnd)
    {
        // Thanks MSVC.
        while (true)
        {
            Iter1 Iter = begin;
            for (Iter2 fit = findBegin; /* ... */; ++Iter, ++fit)
            {
                if (fit == findEnd)
                    return begin;
                else if (Iter == end)
                    return Iter;

                if (*Iter != *fit)
                    break;
            }

            ++begin;
        }
    }

    // Returns an iterator to the first occurance of a value which satisfies the
    // predicate `pred` in the range `[begin, end]`, or `end` if no occurences were found.
    template<ForwardIterator Iter,
             Invokable<typename IteratorTraits<Iter>::ValueType> Pred>
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

    // Returns an iterator to the last occurance of the value `value` in the range
    // `[begin, end]`, or `end` if no occurences were found.
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

    // Returns an iterator to the last occurance of a value which satisfies the
    // predicate `pred` in the range `[begin, end]`, or `end` if no occurences were found.
    template<ForwardIterator Iter,
             Invokable<typename IteratorTraits<Iter>::ValueType> Pred>
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
