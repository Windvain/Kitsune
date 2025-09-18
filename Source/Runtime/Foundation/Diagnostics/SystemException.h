#pragma once

#include "Foundation/Diagnostics/IException.h"

namespace Kitsune
{
    class SystemException : public IException
    {
    public:
        inline SystemException(const char* desc = "Unknown system error")
            : IException("SystemException", desc)
        {
        }
    };
}
