#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class BadAllocException : public Exception
    {
    public:
        inline BadAllocException()
            : Exception("BadAllocException", "An allocation request failed.")
        {
        }
    };
}
