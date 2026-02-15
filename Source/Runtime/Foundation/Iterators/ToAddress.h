#pragma once

namespace Kitsune
{
    template<typename T>
    constexpr auto ToAddress(const T& iterator)
    {
        return iterator.operator->();
    }

    template<typename T>
    constexpr T* ToAddress(T* pointer)
    {
        return pointer;
    }
}
