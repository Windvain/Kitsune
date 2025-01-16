#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class OutOfRangeException : public IException
    {
    public:
        OutOfRangeException() = default;

    public:
        const char* GetName() const noexcept override { return "OutOfRangeException"; }
        const char* GetDescription() const noexcept override
        {
            return "Requested an out of range index";
        }
    };
}
