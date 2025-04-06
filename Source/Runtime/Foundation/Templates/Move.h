#pragma once

#include <type_traits>

namespace Kitsune
{
    template<typename T>
    [[nodiscard]] constexpr std::remove_reference_t<T>&& Move(T&& val)
    {
        return static_cast<std::remove_reference_t<T>&&>(val);
    }
}
