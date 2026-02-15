#pragma once

#include <type_traits>

namespace Kitsune
{
    // Forwards an r-value as an r-value.
    template<typename T>
    [[nodiscard]]
    constexpr T&& Forward(std::remove_reference_t<T>&& value)
    {
        static_assert(!std::is_lvalue_reference_v<T>,
                      "Cannot forward an r-value as an l-value");

        return static_cast<T&&>(value);
    }

    // Forwards the l-value passed in as an l-value or an r-value.
    template<typename T>
    [[nodiscard]]
    constexpr T&& Forward(std::remove_reference_t<T>& value)
    {
        return static_cast<T&&>(value);
    }
}
