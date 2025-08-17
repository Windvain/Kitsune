#include "Foundation/Diagnostics/IException.h"

#include <cstring>
#include "Foundation/Memory/Memory.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/StackTrace.h"

namespace Kitsune
{
    thread_local bool g_WritingToExceptionStackTrace = false;

    thread_local Uint8 g_ExceptionData[1024];
    thread_local Uint8* g_ExceptionDataPointer = g_ExceptionData;

    // HACK: This is defined in KitsuneLaunch, because Windows DLLs cannot
    //       export thread_local variables.
    extern thread_local StackTrace* g_ExceptionStackTrace;

    IException::IException() noexcept
    {
#if defined(KITSUNE_BUILD_PRODUCTION)
        if (g_WritingToExceptionStackTrace)
            return;

        auto* stackTrace = static_cast<StackTrace*>(Memory::TryAllocate(sizeof(StackTrace)));
        if (stackTrace == nullptr)
            return;

        try
        {
            // Makes sure that if anything throws in the exception constructor,
            // that it doesn't keep calling itself, causing a stack overflow.
            g_WritingToExceptionStackTrace = true;
            g_ExceptionStackTrace = stackTrace;

            Memory::ConstructAt(g_ExceptionStackTrace, MakeStackTrace(1));
            g_WritingToExceptionStackTrace = false;
        }
        catch (...) { /* Just ignore the exception. */ }
#endif
    }

    IException::IException(const char* name, const char* desc) noexcept
        : IException()
    {
        // I apologize profusely to the people who might read this code
        // in the future.
        //
        // The exception data is stored in the following format:
        // ________________________________
        // |         |      |             |
        // | (char*) | Name | Description |
        // |_________|______|_____________|

        static_assert(KITSUNE_ARRAY_SIZE(g_ExceptionData) >= (sizeof(char*) + /* Null terms */ 2),
                      "Exception data isn't large enough to fit internal data.");

        Usize nameSize = std::strlen(name) + 1;
        Uint8* descPtr = g_ExceptionDataPointer + sizeof(char*) + nameSize;

        WriteExceptionData(&descPtr, sizeof(char*));

        WriteExceptionData(name, nameSize);
        g_ExceptionDataPointer[-1] = '\0';

        WriteExceptionData(desc, std::strlen(name) + 1);
        g_ExceptionDataPointer[-1] = '\0';
    }

    IException::~IException() noexcept
    {
#if defined(KITSUNE_BUILD_PRODUCTION)
        if (g_ExceptionStackTrace != nullptr)
        {
            Memory::Delete(g_ExceptionStackTrace);
            g_ExceptionStackTrace = nullptr;
        }
#endif
    }

    const char* IException::GetName() const noexcept
    {
        return reinterpret_cast<const char*>(g_ExceptionData + sizeof(char*));
    }

    const char* IException::GetDescription() const noexcept
    {
        return *reinterpret_cast<const char**>(g_ExceptionData);
    }

    void IException::WriteExceptionData(const void* ptr, Usize bytes)
    {
        Usize remainder = g_ExceptionData + KITSUNE_ARRAY_SIZE(g_ExceptionData)
                        - g_ExceptionDataPointer;

        Usize min = KITSUNE_MIN(remainder, bytes);
        std::memcpy(g_ExceptionDataPointer, ptr, min);

        g_ExceptionDataPointer += min;
    }
}
