#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadCallException : public IException
    {
    public:
        BadCallException() = default;

    public:
        const char* GetName() const noexcept override { return "BadCallException"; }
        const char* GetDescription() const noexcept override
        {
            return "Tried to call Function<T>::operator(Args...) without a target.";
        }
    };
}
