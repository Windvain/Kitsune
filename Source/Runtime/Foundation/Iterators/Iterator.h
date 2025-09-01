#pragma once

#include <concepts>
#include <type_traits>

#include "Foundation/Templates/Forward.h"

#include "Foundation/Concepts/Comparable.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename T>
        using AddReference = T&;

        template<typename T>
        concept CanDereference = requires (T val)
        {
            typename AddReference<T>;
        };
    }

    template<typename It>
    concept Iterator =
        std::default_initializable<It> &&
        std::copyable<It> &&

        requires (It iterator)
        {
            { *iterator } -> Details::CanDereference;
            { ++iterator } -> std::same_as<It&>;
            { iterator++ } -> std::same_as<It>;

            typename IteratorTraits<It>::ValueType;
            typename IteratorTraits<It>::DifferenceType;
        };

    template<typename It, typename T>
    concept WritableIterator =
        Iterator<It> &&
        requires (It iterator, T&& val)
        {
            *(iterator++) = Forward<T>(val);
        };

    template<typename It>
    concept ReadableIterator =
        Iterator<It> &&
        requires (It iterator)
        {
            { *iterator } -> std::same_as<typename IteratorTraits<It>::ValueType&>;
        };

    template<typename It>
    concept ForwardIterator = ReadableIterator<It> &&
                              Equatable<const It, const It>;

    template<typename It>
    concept BidirectionalIterator =
        ForwardIterator<It> &&
        requires (It iterator)
        {
            { --iterator } -> std::same_as<It&>;
            { iterator-- } -> std::same_as<It>;
        };

    template<typename It>
    concept RandomAccessIterator =
        BidirectionalIterator<It> &&
        Comparable<const It, const It> &&
        requires (It iter, const It const_iter, typename IteratorTraits<It>::DifferenceType n)
        {
            { iter += n      } -> std::same_as<It&>;
            { const_iter + n } -> std::same_as<It>;
            { n + const_iter } -> std::same_as<It>;
            { iter -= n      } -> std::same_as<It&>;
            { const_iter - n } -> std::same_as<It>;
            { const_iter[n]  } -> std::same_as<typename IteratorTraits<It>::ValueType&>;
        };
}
