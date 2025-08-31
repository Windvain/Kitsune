#pragma once

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Algorithms/Destroy.h"

namespace Kitsune::Algorithms
{
    template<ForwardIterator InputIt, ForwardIterator OutputIt>
    inline OutputIt UninitializedCopy(InputIt begin, InputIt end, OutputIt outBegin)
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

    template<ForwardIterator InputIt, typename Sz, ForwardIterator OutputIt>
    inline OutputIt UninitializedCopyN(InputIt begin, Sz n, OutputIt outBegin)
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
    inline OutputIt UninitializedMove(InputIt begin, InputIt end, OutputIt outBegin)
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

    template<ForwardIterator InputIt, typename Sz, ForwardIterator OutputIt>
    inline OutputIt UninitializedMoveN(InputIt begin, Sz n, OutputIt outBegin)
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
    inline void UninitializedFill(It begin, It end, const T& value)
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

    template<ForwardIterator It, typename Sz, typename T>
    inline It UninitializedFillN(It begin, Sz n, const T& value)
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
