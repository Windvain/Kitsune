#pragma once

#include "Foundation/Templates/Move.h"

namespace Kitsune
{
    template<typename T>
    void Swap(T& val1, T& val2)
    {
        T temp = Move(val1);
        val1 = Move(val2);
        val2 = Move(temp);
    }
}
