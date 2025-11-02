// The crtdbg.h header only works when compiling with /MDd and /MTd.
#if defined(_MSC_VER) && defined(KITSUNE_BUILD_DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <cstdlib>
    #include <crtdbg.h>
#endif

#include <cstdio>
#include <cwchar>

#include <Windows.h>

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/IException.h"

// Exception codes with no macro definitions in the <Windows.h> header.
// Usually undocumented, so don't ask why Microsoft chose that as an error code.
#define KITSUNE_CXX_EXCEPTION_CODE 0xE06D7363

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
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Misaligned Data Type";
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

    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Non-continuable Exception Occurred";
    case KITSUNE_CXX_EXCEPTION_CODE:         return "C++ Exception";
    default:                                 return "Unknown";
    }
}

KITSUNE_FORCEINLINE bool SetDpiAwareness()
{
#if defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
    // MinGW doesn't load the DPI-aware functions.
    using SetThreadDpiAwarenessContextFunction = DPI_AWARENESS_CONTEXT (*)(DPI_AWARENESS_CONTEXT);

    HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
    SetThreadDpiAwarenessContextFunction setThreadDpiAwarenessContext;

    if (user32 == nullptr)
        return false;

    setThreadDpiAwarenessContext = (SetThreadDpiAwarenessContextFunction)(void*)
                                    (::GetProcAddress(user32, "SetThreadDpiAwarenessContext"));

    return (setThreadDpiAwarenessContext != nullptr) ?
        (setThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != nullptr) :
        false;
#else
    return (::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != nullptr);
#endif
}

inline bool AllocateConsoleInDev()
{
#if defined(KITSUNE_BUILD_PRODUCTION)
    return true;
#else
    // Allocating a console in developer builds for logging, since WinMain() doesn't
    // allocate one.
    BOOL consoleAllocSuccess = ::AllocConsole();
    if (consoleAllocSuccess == 0)
        return false;

    // Redirect stdout, stderr, and stdin to CONIN$ and CONOUT$, because GetStdHandle() is not set
    // in applications not compiling with /SUBSYSTEM:CONSOLE.
    KITSUNE_UNUSED(std::freopen("CONOUT$", "w", stdout));
    KITSUNE_UNUSED(std::freopen("CONOUT$", "w", stderr));
    KITSUNE_UNUSED(std::freopen("CONIN$", "r", stdin));

    HANDLE conout = ::CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE,
                                  nullptr, OPEN_EXISTING, 0, nullptr);

    HANDLE conin = ::CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
                                 nullptr, OPEN_EXISTING, 0, nullptr);

    ::SetStdHandle(STD_OUTPUT_HANDLE, conout);
    ::SetStdHandle(STD_INPUT_HANDLE, conin);
    ::SetStdHandle(STD_ERROR_HANDLE, conout);

    // Enable VT100 terminal sequence parsing.
    DWORD outputConsoleMode;
    DWORD inputConsoleMode;

    ::GetConsoleMode(conout, &outputConsoleMode);
    ::GetConsoleMode(conin, &inputConsoleMode);

    ::SetConsoleMode(conout, outputConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    ::SetConsoleMode(conin, inputConsoleMode | ENABLE_VIRTUAL_TERMINAL_INPUT);

    return true;
#endif
}

DWORD ProcessSehException(LPEXCEPTION_POINTERS exceptionInfo)
{
    PEXCEPTION_RECORD exceptionRecord = exceptionInfo->ExceptionRecord;
    DWORD exceptionCode = exceptionRecord->ExceptionCode;

    std::printf(
        "The engine has been terminated by an SEH exception. (Code: 0x%lx)\n"
        "Description: %s\n",
        exceptionCode,
        FormatExceptionCode(exceptionCode)
    );

    for (DWORD i = 0; i < exceptionRecord->NumberParameters; ++i)
    {
        void* ptr = reinterpret_cast<void*>(exceptionRecord->ExceptionInformation[i]);
        std::printf("Parameter[%lx]: 0x%p\n", i, ptr);
    }

    // Certain exceptions have additional information regarding why it was thrown.
    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-exception_record#members/
    if (exceptionCode == KITSUNE_CXX_EXCEPTION_CODE)
    {
        // https://devblogs.microsoft.com/oldnewthing/20100730-00/?p=13273
        IException& cppException = *reinterpret_cast<IException*>(exceptionRecord->ExceptionInformation[1]);
        std::printf("\nC++ exception name: %s\n"
                    "C++ exception description: %s\n",
                    cppException.GetName(), cppException.GetDescription());
    }
    else if ((exceptionCode == EXCEPTION_ACCESS_VIOLATION) || (exceptionCode == EXCEPTION_IN_PAGE_ERROR))
    {
        ULONG_PTR rwx = exceptionRecord->ExceptionInformation[0];
        const char* desc;

        switch (rwx)
        {
        case 0: desc = "Attempted to write to an inaccessible address"; break;
        case 1: desc = "Attempted to read to an inaccessible address"; break;
        case 8: desc = "The thread caused a user-mode data execution prevention violation"; break;
        default:
            KITSUNE_UNREACHABLE();
        };

        std::printf("\nAccess violation description: %s\n"
                    "Virtual data accessed: 0x%p",
                    desc, reinterpret_cast<void*>(exceptionRecord->ExceptionInformation[1]));
    }

    return EXCEPTION_CONTINUE_SEARCH;       // Continue finding exception filters.
}

int StartWindowsEntry()
{
    int returnValue = 0;

    // MinGW doesn't seem to define the _CRTDBG_LEAK_CHECK_DF macro.
    // In addition to that, most of the <crtdbg.h> functions are just macros to (void)0.
#if defined(KITSUNE_COMPILER_MSVC) && defined(KITSUNE_BUILD_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

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
        returnValue = EngineMain(argc, argv);
    }
    else
    {
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
    }

    // Makes debugging much, much easier. Gives you a chance to look at the stacktrace
    // that the program produces.
#if !defined(KITSUNE_BUILD_PRODUCTION)
    if (returnValue != 0)
        ::Sleep(INFINITE);
#endif

    return returnValue;
}

void ShutdownWindowsEntry()
{
#if !defined(KITSUNE_BUILD_PRODUCTION)
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
