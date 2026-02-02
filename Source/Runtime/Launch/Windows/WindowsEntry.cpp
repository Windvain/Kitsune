#include "Foundation/Common/Predefined.h"

#if !defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN) && defined(KITSUNE_BUILD_DEBUG)
    #define _CRTDBG_MAP_ALLOC
    #include <cstdlib>
    #include <crtdbg.h>
#endif

#include <cstdio>
#include <exception>        // IWYU pragma: keep
#include <Windows.h>

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/Assert.h"

// Exception codes with no macro definitions in the <Windows.h> header.
#define EXCEPTION_CXX_THROW 0xE06D7363

namespace Kitsune
{
    int UniversalMain(int argc, char** argv);
}

static void SetPerMonitorDpiAwareness()
{
    using SetThreadDpiAwarenessCtx = DPI_AWARENESS_CONTEXT (*)(DPI_AWARENESS_CONTEXT);
    SetThreadDpiAwarenessCtx setThreadDpiAwarenessCtx;

#if !defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
    setThreadDpiAwarenessCtx = ::SetThreadDpiAwarenessContext;
#else
    // MinGW doesn't load the DPI-aware functions.
    HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
    if (user32 == nullptr)
        return;

    setThreadDpiAwarenessCtx = (SetThreadDpiAwarenessCtx)(void*)(
        ::GetProcAddress(user32, "SetThreadDpiAwarenessContext"));

    if (setThreadDpiAwarenessCtx == nullptr)
        return;
#endif

    setThreadDpiAwarenessCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
}

#if defined(KITSUNE_TERMINAL_ENABLED_FOR_DEBUGGING)
static bool TryCreateTerminal()
{
    BOOL consoleAllocSuccess = ::AllocConsole();
    if (consoleAllocSuccess == 0)
        return false;

    // Redirect stdout, stderr, and stdin to CONIN$ and CONOUT$, because GetStdHandle()
    // is not set in Win32 GUI applications (/SUBSYSTEM:WINDOWS).
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
}

static void DestroyTerminal()
{
    KITSUNE_VERIFY(::FreeConsole(), "Failed to free the console.");

    // Direct standard streams to NULL just in case.
    KITSUNE_UNUSED(std::freopen("NUL:", "w", stdout));
    KITSUNE_UNUSED(std::freopen("NUL:", "r", stdin));
    KITSUNE_UNUSED(std::freopen("NUL:", "w", stderr));
}
#endif

#if defined(KITSUNE_COMPILER_MSVC)
static const char* FormatExceptionCode(const DWORD code)
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
    case EXCEPTION_CXX_THROW:                return "C++ Exception";

    default:
        return "Unknown";
    }
}

static DWORD ProcessSehException(const LPEXCEPTION_POINTERS exceptionInfo)
{
    PEXCEPTION_RECORD record = exceptionInfo->ExceptionRecord;
    DWORD exceptionCode = record->ExceptionCode;

    std::printf(
        "The engine has been terminated by an SEH exception. (Code: 0x%lx)\n"
        "Description: %s\n",
        exceptionCode,
        FormatExceptionCode(exceptionCode));

    for (DWORD index = 0; index < record->NumberParameters; ++index)
    {
        void* pointer = reinterpret_cast<void*>(record->ExceptionInformation[index]);
        std::printf("Parameter[%lx]: 0x%p\n", index, pointer);
    }

    // Certain exceptions have additional information regarding why it was thrown.
    // https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-exception_record#members/
    if (exceptionCode == EXCEPTION_CXX_THROW)
    {
        // https://devblogs.microsoft.com/oldnewthing/20100730-00/?p=13273
        auto* exception = reinterpret_cast<std::exception*>(record->ExceptionInformation[1]);
        std::printf("\nC++ exception name: %s\n", exception->what());
    }
    else if ((exceptionCode == EXCEPTION_ACCESS_VIOLATION) ||
             (exceptionCode == EXCEPTION_IN_PAGE_ERROR))
    {
        ULONG_PTR filePermissions = record->ExceptionInformation[0];
        const char* description;

        switch (filePermissions)
        {
        case 0:
            description = "Attempted to write to an inaccessible address";
            break;
        case 1:
            description = "Attempted to read to an inaccessible address";
            break;
        case 8:
            description = "The thread caused a user-mode data execution "
                          "prevention violation";
            break;

        default:
            KITSUNE_UNREACHABLE();
        };

        std::printf(
            "\nAccess violation description: %s\nVirtual data accessed: 0x%p",
            description,
            reinterpret_cast<void*>(record->ExceptionInformation[1]));
    }

    return EXCEPTION_CONTINUE_SEARCH;       // Continue finding exception filters.
}
#endif

#if defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
int main()
#else
int WINAPI WinMain(HINSTANCE /* hInstance */, HINSTANCE /* hPrevInstance */,
                   LPSTR /* lpCmdLine */, int /* nShowCmd */)
#endif
{
    int returnValue = EXIT_SUCCESS;

    // MinGW doesn't define the <crtdbg.h> header functions.
    // Enables debug heap allocations (ALLOC_MEM_DF) and automatically dump memory leaks
    // when the program exits (LEAK_CHECK_DF).
#if !defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN) && defined(KITSUNE_BUILD_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    SetPerMonitorDpiAwareness();

#if defined(KITSUNE_TERMINAL_ENABLED_FOR_DEBUGGING)
    if (!TryCreateTerminal())
        return EXIT_FAILURE;
#endif

    if (::IsDebuggerPresent())
        returnValue = Kitsune::UniversalMain(__argc, __argv);
    else
    {
        // Only MSVC has support for SEH (Structured Exception Handling) exceptions.
        //
#if defined(KITSUNE_COMPILER_MSVC)
        __try
#endif
        {
            returnValue = Kitsune::UniversalMain(__argc, __argv);
        }
#if defined(KITSUNE_COMPILER_MSVC)
        __except (ProcessSehException(GetExceptionInformation()))
        {
            KITSUNE_UNREACHABLE();
        }
#endif

        // Makes debugging much easier. Gives me a chance to look at the backtrace that the
        // program produces.
#if !defined(KITSUNE_BUILD_PRODUCTION)
        if (returnValue != 0)
            ::Sleep(INFINITE);
#endif
    }

#if defined(KITSUNE_TERMINAL_ENABLED_FOR_DEBUGGING)
    DestroyTerminal();
#endif

    return returnValue;
}
