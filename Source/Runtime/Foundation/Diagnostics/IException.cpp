#include "Foundation/Diagnostics/IException.h"

#include <cstring>
#include "Foundation/Memory/Memory.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/StackTrace.h"

namespace Kitsune
{
    // Defined in Launch/EngineMain.cpp.
    extern thread_local StackTrace* g_ExceptionStackTrace;

    namespace
    {
        thread_local bool g_WritingToExceptionStackTrace = false;
        thread_local Uint8 g_ExceptionData[1024];
    }

    IException::IException() noexcept
    {
#if !defined(KITSUNE_BUILD_PRODUCTION)
        KITSUNE_UNUSED(g_WritingToExceptionStackTrace);
#else
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

            Memory::ConstructAt(g_ExceptionStackTrace, StackTrace::Current());
            g_WritingToExceptionStackTrace = false;
        }
        catch (...)
        {
            // Just ignore the exception.
        }
#endif
    }

    IException::IException(const char* name, const char* desc) noexcept
        : IException()
    {
        Uint8* pointer = g_ExceptionData;
        Uint64 nameLength = std::strlen(name);

        std::memcpy(pointer, &nameLength, sizeof(Uint64));
        pointer += sizeof(Uint64);

        std::memcpy(pointer, name, sizeof(char) * (nameLength + 1));
        pointer += (nameLength + 1);

        std::memcpy(pointer, desc, sizeof(char) * (std::strlen(desc) + 1));
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
        return reinterpret_cast<const char*>(g_ExceptionData + sizeof(Uint64));
    }

    const char* IException::GetDescription() const noexcept
    {
        Uint64 nameLength = *reinterpret_cast<Uint64*>(g_ExceptionData);
        return reinterpret_cast<const char*>(g_ExceptionData + sizeof(Uint64) + nameLength + 1);
    }
}
