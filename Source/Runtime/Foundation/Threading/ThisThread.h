#pragma once

#include "Foundation/Diagnostics/StackTrace.h"

namespace Kitsune
{
    class ThisThread
    {
    public:
        KITSUNE_API_ static StackTrace* GetExceptionStackTrace();
        KITSUNE_API_ static void SetExceptionStackTrace(StackTrace* stackTrace);

    private:
        static thread_local StackTrace* s_ExceptionStackTrace;
    };
}
