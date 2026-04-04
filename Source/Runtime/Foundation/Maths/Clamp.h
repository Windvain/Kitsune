#pragma once

#include "Foundation/Concepts/Invocable.h"

namespace Kitsune::Maths
{
    template<typename T>
    inline const T& Clamp(const T& value, const T& minimumValue, const T& maximumValue)
    {
        return Clamp(
            value, minimumValue, maximumValue,
            [](const T& lhs, const T& rhs) -> bool
            {
                return (lhs < rhs);
            });
    }

    template<typename T, InvocableReturn<bool, const T&, const T&> Compare>
    inline const T& Clamp(const T& value, const T& minimumValue, const T& maximumValue,
                          Compare compare)
    {
        return compare(value, minimumValue) ? minimumValue :
               compare(maximumValue, value) ? maximumValue :
                                              value;
    }
}
