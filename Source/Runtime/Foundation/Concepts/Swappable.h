#pragma once

#include "Foundation/Templates/Swap.h"

namespace Kitsune
{
    template<typename T>
    concept Swappable = requires (T& lhs, T& rhs)
    {
        Swap(lhs, rhs);
    };

    template<typename T, typename U>
    concept SwappableWith = requires (T& lhs, U& rhs)
    {
        Swap(lhs, rhs);
        Swap(rhs, lhs);
    };
}
