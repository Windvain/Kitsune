#pragma once

#include <concepts>

#include "Foundation/Templates/Forward.h"
#include "Foundation/Concepts/Comparable.h"

#include "Foundation/Iterators/ToAddress.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename T>
        using AddReference_ = T&;

        template<typename T>
        concept CanReference_ = requires (T val)
        {
            typename AddReference_<T>;
        };
    }

    // The base concept of an iterator.
    template<typename Iter>
    concept Iterator =
        std::default_initializable<Iter> &&
        std::copyable<Iter> &&
        requires (Iter iterator)
        {
            { *iterator  } -> Details::CanReference_;
            { ++iterator } -> std::same_as<Iter&>;
            { iterator++ } -> std::same_as<Iter>;

            typename IteratorTraits<Iter>::ValueType;
            typename IteratorTraits<Iter>::DifferenceType;
        };

    // Describes an iterator which provides a mechanism for writing/outputting.
    template<typename Iter, typename T>
    concept OutputIterator =
        Iterator<Iter> &&
        requires (Iter iterator, T&& val)
        {
            *(iterator++) = Forward<T>(val);
        };

    // Describes an iterator which provides a mechanism for reading data.
    template<typename Iter>
    concept InputIterator =
        Iterator<Iter> &&
        requires (Iter iterator)
        {
            { *iterator } -> std::same_as<typename IteratorTraits<Iter>::ValueType&>;
        };

    // Describes an `InputIterator` which can be compared with itself.
    template<typename Iter>
    concept ForwardIterator = InputIterator<Iter> &&
                              Equatable<const Iter, const Iter>;

    // Describes a `ForwardIterator` which can be incremented and decremented.
    template<typename Iter>
    concept BidirectionalIterator =
        ForwardIterator<Iter> &&
        requires (Iter iterator)
        {
            { --iterator } -> std::same_as<Iter&>;
            { iterator-- } -> std::same_as<Iter>;
        };

    // Describes a `BidirectionalIterator` which provides constant time
    // advancement of the iterator.
    template<typename Iter>
    concept RandomAccessIterator =
        BidirectionalIterator<Iter> &&
        Comparable<const Iter, const Iter> &&
        requires (Iter iter, const Iter const_iter,
                  typename IteratorTraits<Iter>::DifferenceType n)
        {
            { iter += n } -> std::same_as<Iter&>;
            { const_iter + n } -> std::same_as<Iter>;
            { n + const_iter } -> std::same_as<Iter>;
            { iter -= n } -> std::same_as<Iter&>;
            { const_iter - n } -> std::same_as<Iter>;
            { const_iter - const_iter } -> std::same_as<decltype(n)>;

            { const_iter[n] } -> std::same_as<typename IteratorTraits<Iter>::ValueType&>;

            {
                ToAddress(const_iter)
            } -> std::same_as<typename IteratorTraits<Iter>::ValueType*>;
        };
}
