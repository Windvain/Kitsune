#pragma once

#include "Foundation/Templates/Move.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename T>
        concept HasSwapMemberFunction = requires (T object)
        {
            object.Swap(object);
        };
    }

    template<typename T>
        requires (!Details::HasSwapMemberFunction<T>)
    inline void Swap(T& lhs, T& rhs)
    {
        T tmp = Move(lhs);
        lhs = Move(rhs);
        rhs = Move(tmp);
    }

    template<typename T>
        requires Details::HasSwapMemberFunction<T>
    inline void Swap(T& lhs, T& rhs)
    {
        lhs.Swap(rhs);
    }

    template<ForwardIterator It1, ForwardIterator It2>
    inline void IteratorSwap(It1 it1, It2 it2)
    {
        Swap(*it1, *it2);
    }
}
