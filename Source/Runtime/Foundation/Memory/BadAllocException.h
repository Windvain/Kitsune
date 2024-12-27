#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadAllocException : public IException
    {
    public:
        BadAllocException() = default;

    public:
        const char* GetName() const override { return "BadAllocException"; }
        const char* GetDescription() const override
        {
            return "An allocation request failed";
        }
    };
}
