#pragma once

namespace Kitsune
{
    // Gets the address of the value pointed to by the iterator.
    template<typename T>
    constexpr auto ToAddress(const T& iterator)
    {
        return iterator.operator->();
    }

    // Gets the address of the value pointed to by the iterator.
    template<typename T>
    constexpr T* ToAddress(T* pointer)
    {
        return pointer;
    }
}
