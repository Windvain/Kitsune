#include <cstdio>
#include <cwchar>
#include <cstdlib>

#include <Windows.h>

#include "Foundation/Common/Macros.h"
#include "Foundation/Common/Features.h"
#include "Foundation/Common/Predefined.h"

#include "Foundation/Diagnostics/BadAllocException.h"

namespace Kitsune
{
    extern int EngineMain(int argc, char** argv);
}

using namespace Kitsune;

struct ScopedCommandLineArgs
{
    ScopedCommandLineArgs()
    {
        // Get wide command line arguments.
        LPWSTR cmdLine = ::GetCommandLineW();
        wchar_t** wargv = ::CommandLineToArgvW(cmdLine, &Count);

        // Translate to UTF-8 argv.
        Arguments = (char**)std::malloc(Count * sizeof(char*));
        if (Arguments == nullptr)
            throw BadAllocException();

        for (int i = 0; i < Count; ++i)
        {
            int wideLen = static_cast<int>(std::wcslen(wargv[i]));
            int length = ::WideCharToMultiByte(CP_UTF8, 0, wargv[i], wideLen,
                nullptr, 0, nullptr, nullptr);

            Arguments[i] = (char*)std::malloc((length + 1) * sizeof(char));
            if (Arguments[i] == nullptr)
            {
                for (int j = 0; j < i; ++j)
                    std::free(Arguments[j]);

                std::free(Arguments);
                throw BadAllocException();
            }

            ::WideCharToMultiByte(CP_UTF8, 0, wargv[i], wideLen, Arguments[i],
                (length + 1) * sizeof(char), 0, 0);
        }

        // Free ::CommandLineToArgvW().
        ::LocalFree(wargv);
    }

    ~ScopedCommandLineArgs()
    {
        for (int i = 0; i < Count; ++i)
            std::free(Arguments[i]);

        std::free(Arguments);
    }

    int Count;
    char** Arguments;
};

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
    if ((::SetProcessDPIAware() == 0) || (!AllocateConsoleInDev()))
        return 1;

    ScopedCommandLineArgs cmdLineArgs;

    if (::IsDebuggerPresent())
        return EngineMain(cmdLineArgs.Count, cmdLineArgs.Arguments);

    // MinGW doesn't support SEH (Structured Exception Handling).
    // Only Microsoft VC++ and Borland *really* support SEH.
#if defined(KITSUNE_COMPILER_SUPPORTS_SEH)
    __try
#endif
    {
        returnValue = EngineMain(cmdLineArgs.Count, cmdLineArgs.Arguments);
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
    ::FreeConsole();
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
