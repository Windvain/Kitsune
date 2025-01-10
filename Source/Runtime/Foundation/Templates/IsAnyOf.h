#pragma once

#include <type_traits>

namespace Kitsune
{
    template<typename T, typename... Args>
    static constexpr bool IsAnyOf = (std::is_same_v<T, Args> || ...);
}
