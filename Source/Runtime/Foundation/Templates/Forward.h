#pragma once

#include <type_traits>

namespace Kitsune
{
    template<typename T>
    constexpr T&& Forward(std::remove_reference_t<T>&& val) { return static_cast<T&&>(val); }

    template<typename T>
    constexpr T&& Forward(std::remove_reference_t<T>& val) { return static_cast<T&&>(val); }
}
