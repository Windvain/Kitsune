#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Templates/Move.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"

#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Algorithms/Destroy.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedCopy(InputIt begin, InputIt end, OutputIt outBegin)
    {
        InputIt it = begin;
        try
        {
            for (; it != end; ++it, ++outBegin)
                Memory::ConstructAt(AddressOf(*outBegin), *it);

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, it);
            throw;
        }
    }

    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedCopyN(InputIt begin, Usize n, OutputIt outBegin)
    {
        InputIt it = begin;
        try
        {
            for (; n > 0; ++it, --n, ++outBegin)
                Memory::ConstructAt(AddressOf(*outBegin), *it);

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, it);
            throw;
        }
    }

    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedMove(InputIt begin, InputIt end, OutputIt outBegin)
    {
        InputIt it = begin;
        try
        {
            for (; it != end; ++it, ++outBegin)
                Memory::ConstructAt(AddressOf(*outBegin), Move(*it));

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, it);
            throw;
        }
    }

    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    OutputIt UninitializedMoveN(InputIt begin, Usize n, OutputIt outBegin)
    {
        InputIt it = begin;
        try
        {
            for (; n > 0; ++it, --n, ++outBegin)
                Memory::ConstructAt(AddressOf(*outBegin), Move(*it));

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, it);
            throw;
        }
    }

    template<ForwardIterator It, typename T>
    void UninitializedFill(It begin, It end, const T& value)
    {
        It it = begin;
        try
        {
            for (; it != end; ++it)
                Memory::ConstructAt(AddressOf(*it), value);
        }
        catch (...)
        {
            Algorithms::Destroy(begin, it);
            throw;
        }
    }

    template<ForwardIterator It, typename T>
    It UninitializedFillN(It begin, Usize n, const T& value)
    {
        It it = begin;
        try
        {
            for (; n > 0; --n, ++it)
                Memory::ConstructAt(AddressOf(*it), value);

            return it;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, it);
            throw;
        }
    }
}
