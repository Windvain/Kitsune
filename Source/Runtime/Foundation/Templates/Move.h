#pragma once

#include <type_traits>

namespace Kitsune
{
    // "Moves" or static_cast's the specified value into an r-value.
    template<typename T>
    [[nodiscard]]
    constexpr std::remove_reference_t<T>&& Move(T&& value)
    {
        return static_cast<std::remove_reference_t<T>&&>(value);
    }
}
