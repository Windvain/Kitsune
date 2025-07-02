#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class BadAllocException : public IException
    {
    public:
        inline BadAllocException()
            : IException("BadAllocException", "An allocation request failed")
        {
        }
    };
}
