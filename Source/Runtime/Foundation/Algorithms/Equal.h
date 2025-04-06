#pragma once

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It1, ForwardIterator It2>
    [[nodiscard]] bool Equal(It1 begin1, It1 end1, It2 begin2)
    {
        using ValueType1 = IteratorTraits<It1>::ValueType;
        using ValueType2 = IteratorTraits<It2>::ValueType;

        return Equal(begin1, end1, begin2,
            [](const ValueType1& val1, const ValueType2& val2)
            {
                return (val1 == val2);
            });
    }

    template<ForwardIterator It1, ForwardIterator It2, typename Pred>
    [[nodiscard]] bool Equal(It1 begin1, It1 end1, It2 begin2, Pred pred)
    {
        for (; begin1 != end1; ++begin1, ++begin2)
        {
            if (!pred(*begin1, *begin2))
                return false;
        }

        return true;
    }

    template<ForwardIterator It1, ForwardIterator It2>
    [[nodiscard]] bool Equal(It1 begin1, It1 end1, It2 begin2, It2 end2)
    {
        using ValueType1 = IteratorTraits<It1>::ValueType;
        using ValueType2 = IteratorTraits<It2>::ValueType;

        return Equal(begin1, end1, begin2, end2,
            [](const ValueType1& val1, const ValueType2& val2)
            {
                return (val1 == val2);
            });
    }

    template<RandomAccessIterator It1, RandomAccessIterator It2, typename Pred>
    [[nodiscard]] bool Equal(It1 begin1, It1 end1, It2 begin2, It2 end2, Pred pred)
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

    template<ForwardIterator It1, ForwardIterator It2, typename Pred>
    [[nodiscard]] bool Equal(It1 begin1, It1 end1, It2 begin2, It2 end2, Pred pred)
    {
        for (; (begin1 != end1) && (begin2 != end2); ++begin1, ++begin2)
        {
            if (!pred(*begin1, *begin2))
                return false;
        }

        return (begin1 == end1) && (begin2 == end2);
    }
}
