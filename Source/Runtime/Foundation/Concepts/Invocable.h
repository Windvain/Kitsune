#pragma once

#include <type_traits>

namespace Kitsune
{
    // Specifies that the type `T` is invocable (callable) with
    // the argument types `Args`.
    template<typename T, typename... Args>
    concept Invocable = std::is_invocable_v<T, Args...>;

    // Specifies that the type `T` is invocable with the argument types
    // `Args` and returns the type `Return`.
    template<typename T, typename Return, typename... Args>
    concept InvocableReturn = std::is_invocable_r_v<Return, T, Args...>;
}
