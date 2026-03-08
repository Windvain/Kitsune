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
    template<typename It>
    concept Iterator =
        std::default_initializable<It> &&
        std::copyable<It> &&
        requires (It iterator)
        {
            { *iterator  } -> Details::CanReference_;
            { ++iterator } -> std::same_as<It&>;
            { iterator++ } -> std::same_as<It>;

            typename IteratorTraits<It>::ValueType;
            typename IteratorTraits<It>::DifferenceType;
        };

    // Describes an iterator which provides a mechanism for writing/outputting.
    template<typename It, typename T>
    concept OutputIterator =
        Iterator<It> &&
        requires (It iterator, T&& val)
        {
            *(iterator++) = Forward<T>(val);
        };

    // Describes an iterator which provides a mechanism for reading data.
    template<typename It>
    concept InputIterator =
        Iterator<It> &&
        requires (It iterator)
        {
            { *iterator } -> std::same_as<typename IteratorTraits<It>::ValueType&>;
        };

    // Describes an `InputIterator` which can be compared with itself.
    template<typename It>
    concept ForwardIterator = InputIterator<It> &&
                              Equatable<const It, const It>;

    // Describes a `ForwardIterator` which can be incremented and decremented.
    template<typename It>
    concept BidirectionalIterator =
        ForwardIterator<It> &&
        requires (It iterator)
        {
            { --iterator } -> std::same_as<It&>;
            { iterator-- } -> std::same_as<It>;
        };

    // Describes a `BidirectionalIterator` which provides constant time advancement of
    // the iterator.
    template<typename It>
    concept RandomAccessIterator =
        BidirectionalIterator<It> &&
        Comparable<const It, const It> &&
        requires (It iter, const It const_iter,
                  typename IteratorTraits<It>::DifferenceType n)
        {
            { iter += n               } -> std::same_as<It&>;
            { const_iter + n          } -> std::same_as<It>;
            { n + const_iter          } -> std::same_as<It>;
            { iter -= n               } -> std::same_as<It&>;
            { const_iter - n          } -> std::same_as<It>;
            { const_iter - const_iter } -> std::same_as<decltype(n)>;

            { const_iter[n] }         -> std::same_as<typename IteratorTraits<It>::ValueType&>;
            { ToAddress(const_iter) } -> std::same_as<typename IteratorTraits<It>::ValueType*>;
        };
}
