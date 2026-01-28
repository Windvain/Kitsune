#pragma once

#include <concepts>

namespace Kitsune
{
    // Specifies that the type `T` is invokable (callable) with
    // the argument types `Args`.
    template<typename T, typename... Args>
    concept Invokable = requires (T func, Args... args)
    {
        func(args...);
    };

    // Specifies that the type `T` is invokable with the argument types
    // `Args` and returns the type `Return`.
    template<typename T, typename Return, typename... Args>
    concept InvokableReturn = requires (T func, Args... args)
    {
        { func(args...) } -> std::same_as<Return>;
    };
}
