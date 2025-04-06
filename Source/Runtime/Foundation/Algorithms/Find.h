#pragma once

#include "Foundation/Iterators/IteratorTraits.h"
#include "Foundation/Iterators/Iterator.h"

#include "Foundation/Algorithms/Equal.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It, typename T>
    [[nodiscard]] It Find(It begin, It end, const T& val)
    {
        using ValueType = IteratorTraits<It>::ValueType;
        return FindIf(begin, end, [&val](const ValueType& elem)
        {
            return (val == elem);
        });
    }

    template<ForwardIterator It1, ForwardIterator It2>
    [[nodiscard]] It1 Find(It1 begin, It1 end, It2 findBegin, It2 findEnd)
    {
        // Thanks MSVC.
        while (true)
        {
            It1 it = begin;
            for (It2 fit = findBegin; ; ++it, ++fit)
            {
                if (fit == findEnd) return begin;
                if (it == end) return it;

                if (*it != *fit)
                    break;
            }

            ++begin;
        }
    }

    template<ForwardIterator It, typename Pred>
    [[nodiscard]] It FindIf(It begin, It end, Pred pred)
    {
        for (; begin != end; ++begin)
            if (pred(*begin)) { return begin; }

        return begin;
    }


    template<ForwardIterator It, typename T>
    [[nodiscard]] It FindLast(It begin, It end, const T& val)
    {
        using ValueType = IteratorTraits<It>::ValueType;
        return FindLastIf(begin, end, [&val](const ValueType& elem) -> bool
        {
            return (val == elem);
        });
    }

    template<ForwardIterator It, typename Pred>
    [[nodiscard]] It FindLastIf(It begin, It end, Pred pred)
    {
        It last = end;
        for (; begin != end; ++begin)
            if (pred(*begin)) last = begin;

        return last;
    }
}
