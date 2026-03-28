#include "Foundation/Diagnostics/Backtrace.h"

#include <Windows.h>
#include <DbgHelp.h>

namespace Kitsune
{
#if defined(KITSUNE_SUPPORTS_BACKTRACES)
    KITSUNE_NOINLINE
    static Backtrace::ContainerType GuardedBacktraceCapture(
        Uint32 skipCount, Uint32 maxDepth) noexcept
    {
        const Uint32 MaxCapturedFrames = 128;
        maxDepth = KITSUNE_MIN(maxDepth, MaxCapturedFrames);

        // Capture the backtrace.
        Array<void*, Backtrace::AllocatorType> stackTrace(maxDepth, 0);
        USHORT frameCount = ::CaptureStackBackTrace(
            skipCount,
            maxDepth,
            stackTrace.Data(),
            nullptr);

        stackTrace.Remove(stackTrace.GetBegin() + frameCount, stackTrace.GetEnd());

        // Get the symbol information out of the pointers.
        Backtrace::ContainerType backtraceArray;
        for (void* pointer : stackTrace)
        {
            auto address = reinterpret_cast<DWORD_PTR>(pointer);

            String symbolName = "<unknown>";
            String fileName = "<unknown>";

            Uint64 lineNumber = 0;
            void* funcAddress = nullptr;

            const Usize MaxSymbolNameLength = 256;
            Uint8 symbolInfoBuffer[sizeof(SYMBOL_INFO) + MaxSymbolNameLength] = { 0 };

            auto* symbolInfo = reinterpret_cast<SYMBOL_INFO*>(symbolInfoBuffer);
            symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbolInfo->MaxNameLen = MaxSymbolNameLength;

            if (::SymFromAddr(::GetCurrentProcess(), address, nullptr, symbolInfo))
                symbolName = symbolInfo->Name;

            IMAGEHLP_LINE64 lineStruct;
            lineStruct.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

            DWORD offset_;
            if (::SymGetLineFromAddr64(::GetCurrentProcess(), address, &offset_,
                                       &lineStruct))
            {
                fileName = lineStruct.FileName;
                lineNumber = lineStruct.LineNumber;
                funcAddress = reinterpret_cast<void*>(lineStruct.Address);
            }

            backtraceArray.EmplaceBack(fileName, symbolName, lineNumber, funcAddress);
        }

        return backtraceArray;
    }

    Backtrace Backtrace::Capture(Uint32 skipCount, Uint32 maxDepth) noexcept
    {
        if (::SymInitialize(::GetCurrentProcess(), nullptr, true))
            ::SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS);
        else
        {
            return { /* ... */ };
        }

        Backtrace::ContainerType backtraceArray;
        try
        {
            // Skip GuardedBacktraceCapture() and Backtrace::Capture().
            backtraceArray = GuardedBacktraceCapture(skipCount + 2, maxDepth);
        }
        catch (...)
        {
        }

        ::SymCleanup(::GetCurrentProcess());
        return Backtrace(Move(backtraceArray));
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
