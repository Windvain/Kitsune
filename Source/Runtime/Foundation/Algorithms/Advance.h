#pragma once

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    // Increments `iterator`, an amount of `distance` times.
    template<InputIterator Iter, typename Distance>
    inline void Advance(Iter& iterator, Distance distance)
    {
        while (distance != 0)
        {
            ++iterator;
            --distance;
        }
    }

    // Increments `iterator`, an amount of `distance` times.
    template<BidirectionalIterator Iter, typename Distance>
    inline void Advance(Iter& iterator, Distance distance)
    {
        // For distances larger than 0.
        while (distance > 0)
        {
            ++iterator;
            --distance;
        }

        // For distances smaller than 0.
        while (distance < 0)
        {
            --iterator;
            ++distance;
        }
    }

    // Increments `iterator`, an amount of `distance` times.
    template<RandomAccessIterator Iter, typename Distance>
    inline void Advance(Iter& iterator, Distance distance)
    {
        iterator += distance;
    }
}
