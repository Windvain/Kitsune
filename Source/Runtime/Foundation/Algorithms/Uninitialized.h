#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Templates/Move.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"
#include "Foundation/Iterators/Iterator.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedCopy(InputIt begin, InputIt end, OutputIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            Memory::ConstructAt(AddressOf(*outBegin), *begin);

        return outBegin;
    }

    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedCopyN(InputIt begin, Usize n, OutputIt outBegin)
    {
        for (; n > 0; ++begin, --n, ++outBegin)
            Memory::ConstructAt(AddressOf(*outBegin), *begin);

        return outBegin;
    }

    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedMove(InputIt begin, InputIt end, OutputIt outBegin)
    {
        for (; begin != end; ++begin, ++outBegin)
            Memory::ConstructAt(AddressOf(*outBegin), Move(*begin));

        return outBegin;
    }

    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedMoveN(InputIt begin, Usize n, OutputIt outBegin)
    {
        for (; n > 0; ++begin, --n, ++outBegin)
            Memory::ConstructAt(AddressOf(*outBegin), Move(*begin));

        return outBegin;
    }

    template<ForwardIterator It, typename T>
    void UninitializedFill(It begin, It end, const T& value)
    {
        for (; begin != end; ++begin)
            Memory::ConstructAt(AddressOf(*begin), value);
    }

    template<ForwardIterator It, typename T>
    It UninitializedFillN(It begin, Usize n, const T& value)
    {
        for (; n > 0; --n, ++begin)
            Memory::ConstructAt(AddressOf(*begin), value);

        return begin;
    }
}
