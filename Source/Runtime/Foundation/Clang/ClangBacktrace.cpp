#include "Foundation/Diagnostics/Backtrace.h"

#if defined(KITSUNE_ENABLE_BACKTRACES)
    #include <cstdlib>
    #include <cxxabi.h>
    #include <backtrace.h>

    #include "Foundation/Common/Macros.h"
    #include "Foundation/Memory/ScopedPtr.h"
#endif

namespace Kitsune
{
#if defined(KITSUNE_ENABLE_BACKTRACES)
    struct BacktraceData
    {
        Uint32 CurrentDepth;
        Uint32 MaxDepth;

        Backtrace::ContainerType BacktraceArray;
    };

    static void BacktraceErrorCallback(void* data, const char* message,
                                       int error)
    {
        KITSUNE_UNUSED(data);
        KITSUNE_UNUSED(message);
        KITSUNE_UNUSED(error);
    }

    static int BacktraceCallback(
        void* untypedData, Uintptr pc, const char* filename,
        int line, const char* mangledName)
    {
        auto* data = reinterpret_cast<BacktraceData*>(untypedData);
        if (data->CurrentDepth == data->MaxDepth)
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

        enum DemangleStatus : int
        {
            Success = 0,
            FailedAllocation = -1,
            InvalidMangledName = -2,
            InvalidArguments = -3
        };

        try
        {
            String symbolName;
            if (mangledName == nullptr)
                symbolName = "<unknown>";
            else
            {
                int demangleStatus;
                ScopedPtr<char, DemangleDeleter> demangledName(
                    ::abi::__cxa_demangle(mangledName, nullptr, nullptr,
                                          &demangleStatus));

                if (demangleStatus == DemangleStatus::Success)
                    symbolName = demangledName.Get();
                else
                    symbolName = mangledName;
            }

            data->BacktraceArray.EmplaceBack(
                filename,
                symbolName,
                line,
                reinterpret_cast<void*>(pc));

            ++data->CurrentDepth;
            return 0;
        }
        catch (...)
        {
            // Anything but 0 will halt the backtrace.
            return 1;
        }
    }

    Backtrace Backtrace::Capture(Uint32 skipCount, Uint32 maxDepth) noexcept
    {
        BacktraceData backtraceData = {
            .CurrentDepth = 0,
            .MaxDepth = maxDepth,
            .BacktraceArray = { /* ... */ }
        };

        static backtrace_state* backtraceState = ::backtrace_create_state(
            nullptr, true, BacktraceErrorCallback, nullptr);

        int result = ::backtrace_full(
            backtraceState,
            static_cast<int>(skipCount + 1),
            BacktraceCallback,
            BacktraceErrorCallback,
            &backtraceData);

        if (result != 0)
            return Backtrace();

        return Backtrace(Move(backtraceData.BacktraceArray));
    }
#else
    Backtrace Backtrace::Capture(Uint32 skipCount, Uint32 maxDepth) noexcept
    {
        KITSUNE_UNUSED(skipCount);
        KITSUNE_UNUSED(maxDepth);

        return Backtrace();
    }
#endif
}
