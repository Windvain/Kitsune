#pragma once

#include <concepts>
#include <type_traits>

#include "Foundation/Templates/Forward.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace Kitsune
{
    namespace Internal
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
            { *iterator } -> Internal::CanDereference;
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
            { *iterator } -> std::convertible_to<typename IteratorTraits<It>::ValueType>;
        };

    template<typename It>
    concept ForwardIterator =
        ReadableIterator<It> &&
        requires (It iter1, It iter2)
        {
            { iter1 == iter2 } -> std::convertible_to<bool>;
            { iter2 == iter1 } -> std::convertible_to<bool>;

            { iter1 != iter2 } -> std::convertible_to<bool>;
            { iter2 != iter1 } -> std::convertible_to<bool>;
        };

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
        requires (It iter, const It const_iter, typename IteratorTraits<It>::DifferenceType n)
        {
            { iter += n      } -> std::same_as<It&>;
            { const_iter + n } -> std::same_as<It>;
            { n + const_iter } -> std::same_as<It>;
            { iter -= n      } -> std::same_as<It&>;
            { const_iter - n } -> std::same_as<It>;
            { const_iter[n]  } -> std::same_as<typename IteratorTraits<It>::ValueType&>;
        } &&
        requires (const It iter1, const It iter2)
        {
            { iter1 >  iter2 } -> std::convertible_to<bool>;
            { iter1 <  iter2 } -> std::convertible_to<bool>;
            { iter1 >= iter2 } -> std::convertible_to<bool>;
            { iter1 >= iter2 } -> std::convertible_to<bool>;

            { iter2 >  iter1 } -> std::convertible_to<bool>;
            { iter2 <  iter1 } -> std::convertible_to<bool>;
            { iter2 >= iter1 } -> std::convertible_to<bool>;
            { iter2 >= iter1 } -> std::convertible_to<bool>;
        };
}
