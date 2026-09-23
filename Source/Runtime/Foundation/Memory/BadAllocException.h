#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown when an allocation request failed.
    class BadAllocException : public Exception
    {
    public:
        inline BadAllocException()
            : Exception("BadAllocException", "An allocation request failed.")
        {
        }
    };
}
