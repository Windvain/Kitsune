#pragma once

#include "Foundation/Templates/Forward.h"

namespace Kitsune
{
    template<typename Return, typename T, typename... Args>
    [[nodiscard]]
    constexpr auto MemberFunction(Return (T::*memFuncPtr)(Args...))
    {
        return [memFuncPtr](T& object, Args&&... args)
        {
            return (object.*memFuncPtr)(Forward<Args>(args)...);
        };
    }
}
