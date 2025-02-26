#include <cstdio>
#include <cwchar>
#include <cstdlib>

#include <Windows.h>

#include "Foundation/Common/Macros.h"
#include "Foundation/Common/Features.h"
#include "Foundation/Common/Predefined.h"

#include "Foundation/Memory/BadAllocException.h"

namespace Kitsune
{
    extern int EngineMain(int argc, char** argv);
}

using namespace Kitsune;

const char* FormatExceptionCode(DWORD code)
{
    switch (code)
    {
    case EXCEPTION_BREAKPOINT:               return "Breakpoint Triggered";
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Misaligned Datatype";
    case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal Instruction";

    case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Floating-point Operation on Denormal Number";
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Floating-point Division by 0";
    case EXCEPTION_FLT_INEXACT_RESULT:       return "Floating-point Result Inexact";
    case EXCEPTION_FLT_OVERFLOW:             return "Floating-point Overflow";
    case EXCEPTION_FLT_UNDERFLOW:            return "Floating-point Underflow";
    case EXCEPTION_FLT_STACK_CHECK:          return "Stack Overflow due to Floating-point Operation";
    case EXCEPTION_FLT_INVALID_OPERATION:    return "Unknown Floating-point Error";

    case EXCEPTION_ACCESS_VIOLATION:         return "Access Violation";
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array Bounds Exceeded";
    case EXCEPTION_IN_PAGE_ERROR:            return "Accessed Non-Present Page";

    case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer Division by 0";
    case EXCEPTION_INT_OVERFLOW:             return "Integer Overflow";

    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Non-continuable Exception Occured";
    default:                                 return "Unknown";
    }
}

KITSUNE_FORCEINLINE bool SetDpiAwareness()
{
#if defined(KITSUNE_COMPILER_MSVC)
    return (::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != nullptr);
#else
    return (::SetProcessDPIAware() != 0);
#endif
}

inline bool AllocateConsoleInDev()
{
#if defined(KITSUNE_BUILD_RELEASE)
    return true;
#else
    // Allocating a console in developer builds for logging, since WinMain() doesn't
    // allocate one.
    BOOL consoleAllocSuccess = ::AllocConsole();
    if (consoleAllocSuccess == 0)
        return false;

    std::freopen("CONOUT$", "w", stdout);
    std::freopen("CONOUT$", "w", stderr);
    std::freopen("CONIN$", "r", stdin);

    // Enable VT100 terminal sequence parsing.
    HANDLE stdOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE stdInput = ::GetStdHandle(STD_INPUT_HANDLE);
    HANDLE stdError = ::GetStdHandle(STD_ERROR_HANDLE);

    DWORD outputConsoleMode;
    DWORD inputConsoleMode;
    DWORD errorConsoleMode;

    ::GetConsoleMode(stdOutput, &outputConsoleMode);
    ::GetConsoleMode(stdOutput, &inputConsoleMode);
    ::GetConsoleMode(stdOutput, &errorConsoleMode);

    ::SetConsoleMode(stdOutput, outputConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    ::SetConsoleMode(stdInput,  inputConsoleMode  | ENABLE_VIRTUAL_TERMINAL_INPUT);
    ::SetConsoleMode(stdError,  errorConsoleMode  | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    return true;
#endif
}

DWORD ProcessSehException(LPEXCEPTION_POINTERS exceptionInfo)
{
    PEXCEPTION_RECORD exceptionRecord = exceptionInfo->ExceptionRecord;
    DWORD exceptionCode = exceptionRecord->ExceptionCode;

    std::printf(
        "The engine has been terminated by an SEH exception. (Code: 0x%lx)\n"
        "Description: %s",
        exceptionCode,
        FormatExceptionCode(exceptionCode)
    );

    return EXCEPTION_CONTINUE_SEARCH;       // Continue finding exception filters.
}

int StartWindowsEntry()
{
    int returnValue = 0;

    if ((!SetDpiAwareness()) || (!AllocateConsoleInDev()))
        return 1;

    // Most things passed in via the terminal will be in ASCII, so for now I don't see a need
    // to transcode the UTF-16 output of CommandLineToArgvW() to UTF-8.
    // If unicode support is ever needed in the terminal, do not rollback the change here,
    // that implementation is incorrect and messy.
    int argc = __argc;
    char** argv = __argv;

    if (::IsDebuggerPresent())
    {
        return EngineMain(argc, argv);
    }

    // MinGW doesn't support SEH (Structured Exception Handling).
    // Only Microsoft VC++ and Borland *really* support SEH.
#if defined(KITSUNE_COMPILER_SUPPORTS_SEH)
    __try
#endif
    {
        returnValue = EngineMain(argc, argv);
    }
#if defined(KITSUNE_COMPILER_SUPPORTS_SEH)
    __except (ProcessSehException(GetExceptionInformation()))
    {
        KITSUNE_UNREACHABLE();
    }
#endif

    return returnValue;
}

void ShutdownWindowsEntry()
{
#if !defined(KITSUNE_BUILD_RELEASE)
    ::FreeConsole();
#endif
}

// MSVC just works as long as a WinMain() function was forward-declared.
// MinGW struggles with 'undefined reference to 'WinMain''.
#if !defined(KITSUNE_COMPILER_MSVC)
int main()
#else
int WINAPI WinMain(HINSTANCE /* hInstance */, HINSTANCE /* hPrevInstance */,
                   char* /* lpCmdLine */, int /* nShowCmd */)
#endif
{
    int success = StartWindowsEntry();
    ShutdownWindowsEntry();

    return success;
}
