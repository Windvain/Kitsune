#pragma once

#include <concepts>

namespace Kitsune
{
    template<typename T, typename U>
    concept Equatable = requires (const T& lhs, const U& rhs)
    {
        { lhs == rhs } -> std::convertible_to<bool>;
        { rhs == lhs } -> std::convertible_to<bool>;
    };

    template<typename T, typename U>
    concept Comparable = requires (const T& lhs, const U& rhs)
    {
        { lhs >  rhs } -> std::convertible_to<bool>;
        { lhs <  rhs } -> std::convertible_to<bool>;
        { lhs >= rhs } -> std::convertible_to<bool>;
        { lhs >= rhs } -> std::convertible_to<bool>;

        { rhs >  lhs } -> std::convertible_to<bool>;
        { rhs <  lhs } -> std::convertible_to<bool>;
        { rhs >= lhs } -> std::convertible_to<bool>;
        { rhs >= lhs } -> std::convertible_to<bool>;
    };
}

