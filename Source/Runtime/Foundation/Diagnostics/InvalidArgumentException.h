#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class InvalidArgumentException : public IException
    {
    public:
        InvalidArgumentException(const char* desc = "Invalid argument passed")
            : IException("InvalidArgumentException", desc)
        {
        }
    };
}
