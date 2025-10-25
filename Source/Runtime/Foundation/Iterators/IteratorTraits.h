#pragma once

#include <utility>

namespace Kitsune
{
    namespace Details
    {
        template<typename It>
        struct GetDifferenceType { /* ... */ };

        template<typename It>
            requires (requires (It iter1, It iter2) { iter1 - iter2; } &&
                !requires { typename It::DifferenceType; })
        struct GetDifferenceType<It>
        {
            using Type = decltype(std::declval<It>() - std::declval<It>());
        };

        template<typename It>
            requires requires { typename It::DifferenceType; }
        struct GetDifferenceType<It>
        {
            using Type = typename It::DifferenceType;
        };

        template<typename It>
        struct GetValueType { /* ... */ };

        template<typename T>
        struct GetValueType<T*> { using Type = T; };

        template<typename It>
            requires requires { typename It::ValueType; }
        struct GetValueType<It>
        {
            using Type = typename It::ValueType;
        };
    }

    template<typename It>
    class IteratorTraits
    {
    public:
        using ValueType = Details::GetValueType<It>::Type;
        using DifferenceType = Details::GetDifferenceType<It>::Type;
    };
}
