#include "Foundation/Diagnostics/Backtrace.h"

#if defined(KITSUNE_SUPPORTS_BACKTRACES)
    #include <cstdlib>
    #include <cxxabi.h>
    #include <backtrace.h>

    #include "Foundation/Common/Macros.h"
    #include "Foundation/Memory/ScopedPtr.h"
#endif

namespace Kitsune
{
#if defined(KITSUNE_SUPPORTS_BACKTRACES)
    struct BacktraceData
    {
        Uint32 CurrentDepth;
        Uint32 MaxDepth;

        Array<BacktraceFrame, Details::BacktraceAllocator> BacktraceArray;
    };

    static void BacktraceErrorCallback(void* /* data */,
                                       const char* /* message*/,
                                       int /* error */)
    {
    }

    static int BacktraceCallback(void* data,
                                 std::uintptr_t programCounter,
                                 const char* filename,
                                 int line,
                                 const char* mangledName)
    {
        auto* backtraceData = reinterpret_cast<BacktraceData*>(data);
        if (backtraceData->CurrentDepth == backtraceData->MaxDepth)
            return 1;

        // Since the symbol name is going to be de-mangled, the nullptr check will
        // be done much later in the code.
        if (filename == nullptr)
            filename = "<unknown>";

        struct DemangleDeleter
        {
            using ValueType = char;
            KITSUNE_FORCEINLINE void operator()(char* pointer)
            {
                std::free(pointer);
            }
        };

        try
        {
            int demangleStatus;
            ScopedPtr<char, DemangleDeleter> namePointer(
                ::abi::__cxa_demangle(mangledName, nullptr, nullptr,
                                      &demangleStatus));

            const char* symbolName = (demangleStatus != -2) ? namePointer.Get() :
                                                              mangledName;

            if (symbolName == nullptr)
                symbolName = "<unknown>";            //< Here!

            backtraceData->BacktraceArray.EmplaceBack(
                filename, symbolName, line,
                reinterpret_cast<void*>(programCounter));

            ++backtraceData->CurrentDepth;
            return 0;
        }
        catch (...)
        {
            // Anything but 0 will halt the backtrace.
            return 1;
        }
    }

    Backtrace Backtrace::Capture(Uint32 skipCount,
                                 Uint32 maxDepth) noexcept
    {
        BacktraceData backtraceData = {
            .CurrentDepth = 0,
            .MaxDepth = maxDepth,

            .BacktraceArray = {}
        };

        static backtrace_state* backtraceState = ::backtrace_create_state(
            nullptr, true, BacktraceErrorCallback, nullptr);

        int result = ::backtrace_full(backtraceState, skipCount + 1,
                                      BacktraceCallback, BacktraceErrorCallback,
                                      &backtraceData);

        if (result != 0)
            return Backtrace();

        return Backtrace(Move(backtraceData.BacktraceArray));
    }
#else
    Backtrace Backtrace::Capture(Uint32 /* skipCount */,
                                 Uint32 /* maxDepth */) noexcept
    {
        return Backtrace();
    }
#endif

    bool Backtrace::IsSupported()
    {
#if defined(KITSUNE_SUPPORTS_BACKTRACES)
        return true;
#else
        return false;
#endif
    }
}
