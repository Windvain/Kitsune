#pragma once

#include <concepts>

namespace Kitsune
{
    // This concept is only satisfied if `T` is equatable with `U` and
    // vice versa.
    template<typename T, typename U>
    concept Equatable = requires (const T& lhs, const U& rhs)
    {
        // C++20 and above will generate an overload to operator!= automatically
        // as long as operator== is defined.
        { lhs == rhs } -> std::convertible_to<bool>;
        { rhs == lhs } -> std::convertible_to<bool>;
    };

    // Specifies that `T` can be fully comparable (>, <, >=, <=, ==, !=)
    // with `U` and vice versa.
    template<typename T, typename U>
    concept Comparable =
        Equatable<T, U> &&
        requires (const T& lhs, const U& rhs)
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

