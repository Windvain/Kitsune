#pragma once

#include <utility>

namespace Kitsune
{
    namespace Details
    {
        template<typename It>
        struct GetDifferenceType_ { /* ... */ };

        template<typename It>
            requires (requires (It iter1, It iter2) { iter1 - iter2; } &&
                !requires { typename It::DifferenceType; })
        struct GetDifferenceType_<It>
        {
            using Type = decltype(std::declval<It>() - std::declval<It>());
        };

        template<typename It>
            requires requires { typename It::DifferenceType; }
        struct GetDifferenceType_<It>
        {
            using Type = typename It::DifferenceType;
        };

        template<typename It>
        struct GetValueType_ { /* ... */ };

        template<typename T>
        struct GetValueType_<T*> { using Type = T; };

        template<typename It>
            requires requires { typename It::ValueType; }
        struct GetValueType_<It>
        {
            using Type = typename It::ValueType;
        };
    }

    template<typename It>
    class IteratorTraits
    {
    public:
        using ValueType = Details::GetValueType_<It>::Type;
        using DifferenceType = Details::GetDifferenceType_<It>::Type;
    };
}
