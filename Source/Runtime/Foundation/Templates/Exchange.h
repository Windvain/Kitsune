#pragma once

#include "Foundation/Templates/Move.h"
#include "Foundation/Templates/Forward.h"

namespace Kitsune
{
    // Replaces the value of the object `value` with a new value `newValue` and
    // returns the old value of the object.
    template<typename T, typename U = T>
    [[nodiscard]]
    inline T Exchange(T& value, U&& newValue)
    {
        T temp = Move(value);
        value = Forward<U>(newValue);

        return temp;
    }
}
