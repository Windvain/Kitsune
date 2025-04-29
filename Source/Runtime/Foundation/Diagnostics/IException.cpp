#include "Foundation/Diagnostics/IException.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Threading/ThisThread.h"

namespace Kitsune
{
    // Makes sure that if anything throws in the exception constructor,
    // that it doesn't keep calling itself, causing a stack overflow.
    thread_local bool g_WritingToExceptionStackTrace = false;

    IException::IException() noexcept
    {
#if defined(KITSUNE_BUILD_RELEASE)
        if (g_WritingToExceptionStackTrace)
            return;

        auto* stackTrace = static_cast<StackTrace*>(Memory::TryAllocate(sizeof(StackTrace)));
        if (stackTrace == nullptr)
            return;

        try
        {
            g_WritingToExceptionStackTrace = true;

            Memory::ConstructAt(stackTrace, MakeStackTrace(1));
            ThisThread::SetExceptionStackTrace(stackTrace);

            g_WritingToExceptionStackTrace = false;
        }
        catch (...) { /* Just ignore the exception. */ }
#endif
    }

    IException::~IException()
    {
#if defined(KITSUNE_BUILD_RELEASE)
        StackTrace* stackTrace = ThisThread::GetExceptionStackTrace();
        if (stackTrace != nullptr)
        {
            Memory::Delete(stackTrace);
            ThisThread::SetExceptionStackTrace(nullptr);
        }
#endif
    }
}
