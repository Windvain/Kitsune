#include "Foundation/Diagnostics/StackTrace.h"
#include "Foundation/Common/Macros.h"

#include <cxxabi.h>         // Required header to use __cxa_demangle(), clang specific.
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/Diagnostics/StackTraceException.h"
#include "Foundation/GeneralPlatform/GeneralStackFrame.h"

// Clang runs on basically all of the modern operating systems. We have to
// test for each one and include the necessary headers..
// Is it messy? Kinda. Does it work? Yes.
#if defined(KITSUNE_OS_WINDOWS)
    #include <Windows.h>
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

namespace Kitsune::Details
{
    struct BacktraceData
    {
        Usize CurrentDepth;
        Usize MaxDepth;

        StackTraceFillCallback Callback;
        void* Data;

        String ErrorMessage;
    };

    inline void BacktraceErrorCallback(void* data, const char* message,
                                       int /* error */)
    {
        BacktraceData* backtraceData = static_cast<BacktraceData*>(data);
        if (!backtraceData->ErrorMessage.IsEmpty())
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

        // This runs in C land, so no exceptions are allowed past this function!
        try
        {
            int status;
            ScopedPtr<char, DemangleDeleter> scopedFree(abi::__cxa_demangle(
                mangledFunction, nullptr, nullptr, &status));

            const char* function = (status != KITSUNE_DEMANGLE_INVALID_MANGLED_NAME) ?
                scopedFree.Get() : mangledFunction;

            // Some names are mangled, some are not (like main()).
            if ((function == nullptr) && (status != KITSUNE_DEMANGLE_INVALID_MANGLED_NAME))
                throw StackTraceException(GetDemangleStatus(status));

            SharedPtr<GeneralStackFrame> frame = MakeShared<GeneralStackFrame>(filename, function, reinterpret_cast<void*>(pc), line);
            typedData->Callback(frame, typedData->Data);

            ++typedData->CurrentDepth;
            return 0;
        }
        catch (...)
        {
            BacktraceErrorCallback(data, "An exception has been thrown in internal code.", /* unused */ 0);
            return 1;
        }
    }

    inline backtrace_state* CreateBacktraceState()
    {
        static backtrace_state* state = ::backtrace_create_state(
            nullptr, 1, BacktraceErrorCallback, nullptr);

        return state;
    }

    void DoBackTrace(Usize skipCount, Usize maxDepth,
                     StackTraceFillCallback callback, void* data)
    {
        backtrace_state* state = CreateBacktraceState();
        BacktraceData backtraceData = {
            .CurrentDepth = 0,
            .MaxDepth = maxDepth,
            .Callback = callback,
            .Data = data,
            .ErrorMessage = ""
        };

        ::backtrace_full(state, skipCount + 1, BacktraceFullCallback, BacktraceErrorCallback, &backtraceData);
        if (!backtraceData.ErrorMessage.IsEmpty())
            throw StackTraceException(backtraceData.ErrorMessage.Raw());
    }
}
