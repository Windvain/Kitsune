#pragma once

#include "Foundation/Templates/Swap.h"

namespace Kitsune
{
    // Specifies that the type `T` is swappable with itself.
    template<typename T>
    concept Swappable = requires (T& lhs, T& rhs)
    {
        Swap(lhs, rhs);
    };

    // Specifies that both of the types `T` and `U` are swappable
    // with each other.
    template<typename T, typename U>
    concept SwappableWith = requires (T& lhs, U& rhs)
    {
        Swap(lhs, rhs);
        Swap(rhs, lhs);
    };
}
