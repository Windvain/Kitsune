#pragma once

#include <concepts>

namespace Kitsune
{
    template<typename T, typename... Args>
    concept Invokable = requires (T func, Args... args)
    {
        func(args...);
    };

    template<typename T, typename Ret, typename... Args>
    concept InvokableReturn = requires (T func, Args... args)
    {
        { func(args...) } -> std::same_as<Ret>;
    };
}
