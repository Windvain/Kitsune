#include "Foundation/Diagnostics/IException.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Threading/ThisThread.h"

namespace Kitsune
{
    IException::IException()
    {
#if defined(KITSUNE_BULD_RELEASE)
        auto* stackTrace = static_cast<StackTrace*>(Memory::TryAllocate(sizeof(StackTrace)));
        if (stackTrace == nullptr)
            return;

        Memory::ConstructAt(stackTrace, MakeStackTrace(1));
        ThisThread::SetExceptionStackTrace(stackTrace);
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
