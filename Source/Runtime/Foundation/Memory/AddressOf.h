#pragma once

#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    // Returns the address of an object, even if the `&` operator
    // has been overriden.
    template<typename T>
    [[nodiscard]]
    inline T* AddressOf(T& object)
    {
#if KITSUNE_HAS_BUILTIN(__builtin_addressof)
        return __builtin_addressof(object);
#else
        return reinterpret_cast<T*>(
            &const_cast<char&>(
                reinterpret_cast<const volatile char&>(object)
            ));
#endif
    }
}
