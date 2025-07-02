#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class WindowException : public IException
    {
    public:
        inline WindowException(const char* desc = "Unknown window error")
            : IException("WindowException", desc)
        {
        }
    };
}
