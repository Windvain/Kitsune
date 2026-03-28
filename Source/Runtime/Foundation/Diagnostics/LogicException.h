#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    // Thrown as a result of faulty logic. For example, shutting a subsystem down
    // before it is even initialized.
    class LogicException : public Exception
    {
    public:
        LogicException(const char* description = "Unknown logic error occurred.")
            : Exception("LogicException", description)
        {
        }
    };
}
