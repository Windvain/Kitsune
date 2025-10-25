#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class OutOfRangeException : public IException
    {
    public:
        inline OutOfRangeException()
            : IException("OutOfRangeException", "Requested an out of range index")
        {
        }
    };
}
