#pragma once

#include "Foundation/Templates/Forward.h"

namespace Kitsune
{
    template<typename Return, typename T, typename... Args>
    constexpr auto MemberFunction(Return (T::*memFuncPtr)(Args...))
    {
        return [memFuncPtr](T& object, Args&&... args) { return (object.*memFuncPtr)(Forward<Args>(args)...); };
    }

    template<typename Return, typename T, typename... Args>
    constexpr auto MemberFunction(T& object, Return (T::*memFuncPtr)(Args...))
    {
        return [&object, memFuncPtr](Args&&... args) { return (object.*memFuncPtr)(Forward<Args>(args)...); };
    }
}
