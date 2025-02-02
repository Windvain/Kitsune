#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class InvalidUnicodeException : public IException
    {
    public:
        InvalidUnicodeException() = default;

    public:
        const char* GetName() const noexcept override { return "InvalidUnicodeException"; }
        const char* GetDescription() const noexcept override
        {
            return "Tried to do an operation with an invalid unicode string";
        }
    };
}
