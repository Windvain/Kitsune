#pragma once

#include <type_traits>

namespace Kitsune
{
    // True if the type `T` is equal to at least one of the types in `Args`, else
    // is false.
    template<typename T, typename... Args>
    static constexpr bool IsAnyOf = (std::is_same_v<T, Args> || ...);
}
