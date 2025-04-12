#include "Foundation/Threading/ThisThread.h"

namespace Kitsune
{
    thread_local StackTrace* ThisThread::s_ExceptionStackTrace = nullptr;

    StackTrace* ThisThread::GetExceptionStackTrace()
    {
        return s_ExceptionStackTrace;
    }

    void ThisThread::SetExceptionStackTrace(StackTrace* stackTrace)
    {
        s_ExceptionStackTrace = stackTrace;
    }
}
