#pragma once

#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    template<typename T>
    [[nodiscard]] T* AddressOf(T& ref)
    {
#if KITSUNE_HAS_BUILTIN(__builtin_addressof)
        return __builtin_addressof(ref);
#else
        return reinterpret_cast<T*>(&const_cast<char&>(
            reinterpret_cast<const volatile char&>(ref)));
#endif
    }
}
