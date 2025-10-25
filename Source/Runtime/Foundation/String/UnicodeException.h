#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class UnicodeException : public IException
    {
    public:
        inline UnicodeException(const char* desc = "Unknown unicode error")
            : IException("UnicodeException", desc)
        {
        }
    };
}
