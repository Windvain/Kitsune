#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadCallException : public IException
    {
    public:
        inline BadCallException()
            : IException(
                "BadCallException",
                "Tried to call Function<T>::operator(Args...) without a target.")
        {
        }
    };
}
