#pragma once

#include "Foundation/Concepts/Invocable.h"

namespace Kitsune::Maths
{
    template<typename T>
    const T& Minimum(const T& value1, const T& value2)
    {
        return Minimum(
            value1, value2,
            [](const T& lhs, const T& rhs) -> bool
            {
                return lhs < rhs;
            });
    }

    template<
        typename T,
        InvocableReturn<bool, const T&, const T&> LessCompare>
    const T& Minimum(const T& value1, const T& value2, LessCompare compare)
    {
        return compare(value1, value2) ? value1 : value2;
    }
}
