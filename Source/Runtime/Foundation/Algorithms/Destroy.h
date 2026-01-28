#pragma once

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    // Destroys the objects in the range `[begin, end]`.
    template<ForwardIterator Iter>
    inline void Destroy(Iter begin, Iter end)
    {
        for (; begin != end; ++begin)
            Memory::DestroyAt(AddressOf(*begin));
    }

    // Destroys the objects in the range `[begin, begin + n]`.
    template<ForwardIterator Iter, typename Size>
    inline Iter DestroyN(Iter begin, Size n)
    {
        for (; n > 0; ++begin, --n)
            Memory::DestroyAt(AddressOf(*begin));

        return begin;
    }
}
