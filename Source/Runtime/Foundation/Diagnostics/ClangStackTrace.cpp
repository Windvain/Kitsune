#include "Foundation/Diagnostics/StackTrace.h"
#include "Foundation/Common/Macros.h"

#include <cxxabi.h>         // Required header to use __cxa_demangle(), clang specific.
#include "Foundation/Memory/ScopedPtr.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/StackTraceException.h"

// Clang runs on basically all of the modern operating systems. We have to
// test for each one and include the necessary headers..
// Is it messy? Kinda. Does it work? Yes.
#if defined(KITSUNE_OS_WINDOWS)
    #include <Windows.h>
    #include "Foundation/String/Format.h"   // To convert ::GetCallingThreadId() to String.
#endif

#if KITSUNE_HAS_INCLUDE(<backtrace.h>)
    #include <backtrace.h>
#else
    #error Could not find <backtrace.h> header. Have you installed `libbacktrace`?
#endif

// Definitions for __cxa_demangle() status codes.
#define KITSUNE_DEMANGLE_SUCCESS 0
#define KITSUNE_DEMANGLE_MEMORY_ALLOC_FAILED -1
#define KITSUNE_DEMANGLE_INVALID_MANGLED_NAME -2
#define KITSUNE_DEMANGLE_INVALID_ARGUMENT -3

namespace Kitsune
{
    struct BacktraceData
    {
        Usize CurrentDepth;
        Usize MaxDepth;

        Array<StackFrame> StackFrames;
        String ErrorMessage;
    };

    inline void BacktraceErrorCallback(void* data, const char* message,
                                       int /* error */)
    {
        BacktraceData* backtraceData = static_cast<BacktraceData*>(data);
        if (backtraceData->ErrorMessage.IsEmpty())
            return;

        backtraceData->ErrorMessage = message;
    }

    inline const char* GetDemangleStatus(int status)
    {
        KITSUNE_ASSERT(status != KITSUNE_DEMANGLE_SUCCESS,
                       "GetDemangleStatus() should only be called with a failed value.");

        constexpr const char* errorMessages[] = {
            "Success. This message should not come up.",
            "Memory allocation failure",
            "Invalid mangled name according to the C++ ABI mangling rules.",
            "Invalid argument passed."
        };

        return errorMessages[-status];
    }

    int BacktraceFullCallback(void* data, Uintptr pc, const char* filename, int line,
                              const char* mangledFunction)
    {
        BacktraceData* typedData = reinterpret_cast<BacktraceData*>(data);
        if ((typedData->CurrentDepth == typedData->MaxDepth) || (filename == nullptr))
            return 1;       // Any non-zero integer is fine.

        struct DemangleDeleter
        {
            using ValueType = char;
            KITSUNE_FORCEINLINE void operator()(char* ptr) { std::free(ptr); }
        };

        int status;
        ScopedPtr<char, DemangleDeleter> scopedFree(abi::__cxa_demangle(
            mangledFunction, nullptr, nullptr, &status));

        const char* function = (status != KITSUNE_DEMANGLE_INVALID_MANGLED_NAME) ?
            scopedFree.Get() : mangledFunction;

        // Some names are mangled, some are not (like main()).
        if ((function == nullptr) && (status != KITSUNE_DEMANGLE_INVALID_MANGLED_NAME))
            throw StackTraceException(GetDemangleStatus(status));

        typedData->StackFrames.EmplaceBack(filename, function, reinterpret_cast<void*>(pc), line);
        ++typedData->CurrentDepth;

        return 0;
    }

    inline backtrace_state* CreateBacktraceState()
    {
        static backtrace_state* state = ::backtrace_create_state(
            nullptr, 1, BacktraceErrorCallback, nullptr);

        return state;
    }

    inline String GetCallingThreadName()
    {
#if defined(KITSUNE_OS_WINDOWS)
        return Format("{0}", ::GetCurrentThreadId());
#else
    #error Clang is not supported on this platform, please switch to another compiler.
#endif
    }

    StackTrace MakeStackTrace(Usize skipCount, Usize maxDepth)
    {
        backtrace_state* state = CreateBacktraceState();
        BacktraceData backtraceData = {
            .CurrentDepth = 0,
            .MaxDepth = maxDepth,
            .StackFrames{},
            .ErrorMessage = ""
        };

        ::backtrace_full(state, skipCount + 1, BacktraceFullCallback, BacktraceErrorCallback, &backtraceData);
        if (!backtraceData.ErrorMessage.IsEmpty())
            throw StackTraceException(backtraceData.ErrorMessage.Raw());

        return StackTrace(Move(backtraceData.StackFrames), GetCallingThreadName());
    }
}
