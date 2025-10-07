#include <cstdio>
#include <cstdlib>

#include <cinttypes>

#include "Launch/IEngineLoop.h"
#include "Launch/DefaultEngineLoop.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Memory/BadAllocException.h"

#include "Foundation/Diagnostics/IException.h"
#include "Foundation/Diagnostics/StackTrace.h"

namespace Kitsune
{
    // This was made into a global variable in order to make it harder
    // for the user to access this information.
    thread_local StackTrace* g_ExceptionStackTrace = nullptr;

    // This is important. UnguardedEngineMain() uses ScopedPtr<T> and SharedPtr<T>.
    // If Memory::Shutdown() were to be called manually, ScopedPtr<T>'s delete call
    // would be invalid.
    class MemorySubsystemGuard
    {
    public:
        MemorySubsystemGuard()
        {
            Memory::InitializeExplicit();
        }

        ~MemorySubsystemGuard()
        {
            Memory::Shutdown();
        }
    };

    // Keep this as an enum, don't turn it into an enum class.
    enum ExitCode : int
    {
        Success = 0,
        Failed = 1,
        InvalidArgument = 2,

        // From this point on, all exit codes will have the first 8 bits be set to 0b00000001.
        // This is to make sure people don't mix up our engine's exit codes with the typical
        // signal() call exit code.
        FailedEngineLoopInit = 0x101,
        ExceptionThrown = 0x201
    };

    inline EngineLoopNotification TranslateToEngineNotification(const IException& exception)
    {
        // There are no direct ways of checking for an exception's type.
        if (std::strcmp(exception.GetName(), "BadAllocException"))
            return EngineLoopNotification::OutOfMemory;

        return EngineLoopNotification::Crash;
    }

    int UnguardedEngineMain(int argc, char** argv)
    {
        MemorySubsystemGuard memoryGuard{};

        int exitCode = ExitCode::Success;
        ScopedPtr<IEngineLoop> engineLoop = MakeScoped<DefaultEngineLoop>();

        // There are two try/catch blocks, the one here and the one
        // in EngineMain(). This one is for catching exceptions in application code,
        // which are still somewhat saveable.
#if defined(KITSUNE_BUILD_PRODUCTION)
        try
#endif
        {
            if (!engineLoop->Initialize(argc, argv))
                return ExitCode::FailedEngineLoopInit;

            exitCode = engineLoop->Run();
            engineLoop->Shutdown();         // Shutdown() shouldn't throw...
        }
#if defined(KITSUNE_BUILD_PRODUCTION)
        catch (const IException& exception)
        {
            engineLoop->Notify(TranslateToEngineNotification(exception));
            engineLoop->Shutdown();

            throw;
        }
#endif

        return exitCode;
    }

    void PrintStackTrace()
    {
        std::printf("\n");
        if (g_ExceptionStackTrace == nullptr)
        {
            std::printf("No stack traces were created.\n");
            return;
        }

        // Message:
        // Dumping C++ stacktrace.
        // ----------
        // [#1] foo()
        //     (bar.cpp:2121)
        // [#2] bar()
        //     (foo.cpp:201)
        // ----------
        std::printf("Dumping C++ stacktrace.\n----------\n");

        Uint32 index = 1;
        for (auto& frame : *g_ExceptionStackTrace)
        {
            std::printf("[#%" PRIu32 "]: %s\n\t(%s:%" PRIu64 ")\n",
                         index, frame.GetSymbolName().Raw(),
                         frame.GetFileName().Raw(), frame.GetLineNumber());

            ++index;
        }

        std::printf("----------\n");
        std::fflush(stdout);
    }

    int EngineMain(int argc, char** argv)
    {
        // Try/catch block for the entire engine, mainly used for exceptions thrown in
        // the engine subsystem initializations.
#if defined(KITSUNE_BUILD_PRODUCTION)
        try
#endif
        {
            return UnguardedEngineMain(argc, argv);
        }
#if defined(KITSUNE_BUILD_PRODUCTION)
        catch (const std::exception& stdException)
        {
            const IException* exception = dynamic_cast<const IException*>(&stdException);
            if (exception != nullptr)
            {
                std::printf("Program crashed due to %s exception.\nDescription: %s\n",
                            exception->GetName(), exception->GetDescription());
            }
            else
            {
                std::printf("Engine crashed due to a std::exception being thrown.\nDescription: %s\n",
                            stdException.what());
            }

            PrintStackTrace();
            return ExitCode::ExceptionThrown;
        }
#endif

        // Tell the compiler to shush.
        KITSUNE_UNREACHABLE();
    }
}
