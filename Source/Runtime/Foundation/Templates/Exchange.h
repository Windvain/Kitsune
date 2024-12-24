#pragma once

#include "Foundation/Templates/Move.h"
#include "Foundation/Templates/Forward.h"

namespace Kitsune
{
    template<typename T, typename U = T>
    T Exchange(T& val, U&& newValue)
    {
        T tmp = Move(val);
        val = Forward<U>(newValue);

        return tmp;
    }
}
