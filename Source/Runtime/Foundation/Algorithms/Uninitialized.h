#pragma once

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Algorithms/Destroy.h"

namespace Kitsune::Algorithms
{
    // Copies the elements in the range `[begin, end]` into the uninitialzed memory range
    // starting with `outBegin`. Returns an iterator pointing to the element one past the last
    // copied element.
    template<ForwardIterator Iter, ForwardIterator OutIter>
    inline OutIter UninitializedCopy(Iter begin, Iter end, OutIter outBegin)
    {
        Iter it = begin;
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

    // Copies the elements in the range `[begin, begin + n]` into the uninitialzed memory range
    // starting with `outBegin`. Returns an iterator pointing to the element one past the last
    // copied element.
    template<ForwardIterator Iter, typename Size, ForwardIterator OutIter>
    inline OutIter UninitializedCopyN(Iter begin, Size n, OutIter outBegin)
    {
        Iter it = begin;
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

    // Moves the elements in the range `[begin, end]` into the uninitialzed memory range
    // starting with `outBegin`. Returns an iterator pointing to the element one past the last
    // copied element.
    template<ForwardIterator Iter, ForwardIterator OutIter>
    inline OutIter UninitializedMove(Iter begin, Iter end, OutIter outBegin)
    {
        Iter it = begin;
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

    // Copies the elements in the range `[begin, begin + n]` into the uninitialzed memory range
    // starting with `outBegin`. Returns an iterator pointing to the element one past the last
    // copied element.
    template<ForwardIterator Iter, typename Size, ForwardIterator OutIter>
    inline OutIter UninitializedMoveN(Iter begin, Size n, OutIter outBegin)
    {
        Iter it = begin;
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

    // Fills the uninitialized memory range `[begin, end]` with the value `value`.
    // Returns an iterator pointing to the element one past the last copied element.
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

    // Fills the uninitialized memory range `[begin, begin + n]` with the value `value`.
    // Returns an iterator pointing to the element one past the last copied element.
    template<ForwardIterator It, typename Size, typename T>
    inline It UninitializedFillN(It begin, Size n, const T& value)
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
