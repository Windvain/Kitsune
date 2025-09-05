#pragma once

namespace Kitsune
{
    template<typename T>
    concept Swappable = requires (T lhs, T rhs)
    {
        lhs.Swap(rhs);
    };
}
