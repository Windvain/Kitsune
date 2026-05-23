#pragma once

#include <concepts>

namespace Kitsune
{
    // True if the type `T` is equal to at least one of the types in `Args`, else
    // is false.
    template<typename T, typename... Args>
    concept AnyOf = (std::same_as<T, Args> || ...);
}
