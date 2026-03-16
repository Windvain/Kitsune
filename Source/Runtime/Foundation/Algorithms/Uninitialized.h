#pragma once

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/AddressOf.h"

#include "Foundation/Templates/Move.h"
#include "Foundation/Iterators/Iterator.h"
#include "Foundation/Algorithms/Destroy.h"

namespace Kitsune::Algorithms
{
    // Copies the elements in the range `[begin, end]` into the uninitialized memory
    // range which starts at the iterator `outBegin`. Returns an iterator pointing
    // to the element one past the last copied element.
    template<ForwardIterator Iter, ForwardIterator OutIter>
    inline OutIter UninitializedCopy(Iter begin, Iter end, OutIter outBegin)
    {
        Iter iter = begin;
        try
        {
            for (; iter != end; ++iter, ++outBegin)
            {
                Memory::ConstructAt<typename IteratorTraits<Iter>::ValueType>(
                    AddressOf(*outBegin), *iter);
            }

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, iter);
            throw;
        }
    }

    // Copies the elements in the range `[begin, begin + n]` into the uninitialized
    // memory range starting with `outBegin`. Returns an iterator pointing to the
    // element one past the last copied element.
    template<ForwardIterator Iter, typename Size, ForwardIterator OutIter>
    inline OutIter UninitializedCopyN(Iter begin, Size n, OutIter outBegin)
    {
        Iter iter = begin;
        try
        {
            for (; n > 0; ++iter, --n, ++outBegin)
            {
                Memory::ConstructAt<typename IteratorTraits<Iter>::ValueType>(
                    AddressOf(*outBegin), *iter);
            }

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, iter);
            throw;
        }
    }

    // Moves the elements in the range `[begin, end]` into the uninitialized memory
    // range starting with `outBegin`. Returns an iterator pointing to the element
    // one past the last copied element.
    template<ForwardIterator Iter, ForwardIterator OutIter>
    inline OutIter UninitializedMove(Iter begin, Iter end, OutIter outBegin)
    {
        Iter iter = begin;
        try
        {
            for (; iter != end; ++iter, ++outBegin)
            {
                Memory::ConstructAt<typename IteratorTraits<Iter>::ValueType>(
                    AddressOf(*outBegin), Move(*iter));
            }

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, iter);
            throw;
        }
    }

    // Copies the elements in the range `[begin, begin + n]` into the uninitialized
    // memory range starting with `outBegin`. Returns an iterator pointing to the
    // element one past the last copied element.
    template<ForwardIterator Iter, typename Size, ForwardIterator OutIter>
    inline OutIter UninitializedMoveN(Iter begin, Size n, OutIter outBegin)
    {
        Iter iter = begin;
        try
        {
            for (; n > 0; ++iter, --n, ++outBegin)
            {
                Memory::ConstructAt<typename IteratorTraits<Iter>::ValueType>(
                    AddressOf(*outBegin), Move(*iter));
            }

            return outBegin;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, iter);
            throw;
        }
    }

    // Fills the uninitialized memory range `[begin, end]` with the value `value`.
    // Returns an iterator pointing to the element one past the last copied element.
    template<ForwardIterator Iter, typename T>
    inline void UninitializedFill(Iter begin, Iter end, const T& value)
    {
        Iter iter = begin;
        try
        {
            for (; iter != end; ++iter)
            {
                Memory::ConstructAt<typename IteratorTraits<Iter>::ValueType>(
                    AddressOf(*iter), value);
            }
        }
        catch (...)
        {
            Algorithms::Destroy(begin, iter);
            throw;
        }
    }

    // Fills the uninitialized memory range `[begin, begin + n]` with the value `value`.
    // Returns an iterator pointing to the element one past the last copied element.
    template<ForwardIterator Iter, typename Size, typename T>
    inline Iter UninitializedFillN(Iter begin, Size n, const T& value)
    {
        Iter iter = begin;
        try
        {
            for (; n > 0; --n, ++iter)
            {
                Memory::ConstructAt<typename IteratorTraits<Iter>::ValueType>(
                    AddressOf(*iter), value);
            }

            return iter;
        }
        catch (...)
        {
            Algorithms::Destroy(begin, iter);
            throw;
        }
    }
}
