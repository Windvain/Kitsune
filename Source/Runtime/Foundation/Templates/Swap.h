#pragma once

#include "Foundation/Templates/Move.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename T>
        concept HasSwapMemberFunction_ = requires (T object)
        {
            object.Swap(object);
        };
    }

    // Swaps the values of the objects `lhs` and `rhs`.
    template<typename T>
        requires (!Details::HasSwapMemberFunction_<T>)
    inline void Swap(T& lhs, T& rhs)
    {
        T tmp = Move(lhs);
        lhs = Move(rhs);
        rhs = Move(tmp);
    }

    // Swaps the values of the objects `lhs` and `rhs`.
    template<typename T>
        requires Details::HasSwapMemberFunction_<T>
    inline void Swap(T& lhs, T& rhs)
    {
        lhs.Swap(rhs);
    }

    // Swaps the values pointed to by the iterators `iter1` and `iter2`.
    template<ForwardIterator Iter1, ForwardIterator Iter2>
    inline void IteratorSwap(Iter1 iter1, Iter2 iter2)
    {
        Swap(*iter1, *iter2);
    }
}
