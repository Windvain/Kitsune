#pragma once

#include "Foundation/Common/Macros.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<typename T>
    void Swap(T& lhs, T& rhs)
    {
        T tmp = Move(lhs);
        lhs = Move(rhs);
        rhs = Move(tmp);
    }

    template<ForwardIterator It1, ForwardIterator It2>
    It2 Swap(It1 begin, It1 end, It2 outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            Swap(*begin, *outBegin);

        return outBegin;
    }

    template<ForwardIterator It1, ForwardIterator It2>
    KITSUNE_FORCEINLINE void IteratorSwap(It1 it1, It2 it2)
    {
        Swap(*it1, *it2);
    }
}
