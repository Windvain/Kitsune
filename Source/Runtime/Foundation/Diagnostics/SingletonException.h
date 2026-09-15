#pragma once

#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class SingletonException : public Exception
    {
    public:
        inline SingletonException()
            : Exception(
                "SingletonException",
                "An instance of the singleton has already been initialized before.")
        {
        }
    };
}
