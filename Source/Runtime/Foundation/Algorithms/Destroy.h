#pragma once

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"

#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator It>
    void Destroy(It begin, It end)
    {
        for (; begin != end; ++begin)
            Memory::DestroyAt(AddressOf(*begin));
    }

    template<ForwardIterator It, typename Sz>
    It DestroyN(It begin, Sz n)
    {
        for (; n > 0; ++begin, --n)
            Memory::DestroyAt(AddressOf(*begin));

        return begin;
    }
}
