#include <cstdio>
#include <cstdlib>

#include <cinttypes>

#include "Launch/IEngineLoop.h"
#include "Launch/DefaultEngineLoop.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Memory/BackupMemoryAllocator.h"

#include "Foundation/Threading/Mutex.h"
#include "Foundation/Diagnostics/IException.h"
#include "Foundation/Diagnostics/StackTrace.h"

namespace Kitsune
{
    // These was made into a global variable in order to make it harder
    // for the user to access this information.
    ScopedPtr<Mutex>* g_ExceptionMutex = nullptr;
    BasicStackTrace<BackupMemoryAllocator>* g_ExceptionStackTrace = nullptr;

    // This is important. UnguardedEngineMain() uses ScopedPtr<T> and SharedPtr<T>.
    // If Memory::Shutdown() were to be called manually, ScopedPtr<T>'s delete call
    // would be invalid.
    class MemorySubsystemGuard
    {
    public:
        inline MemorySubsystemGuard()
            : m_InitSuccess(Memory::InitializeExplicit())
        {
        }

        inline ~MemorySubsystemGuard()
        {
            Memory::Shutdown();
        }

        inline bool Success() const { return m_InitSuccess; }

    private:
        bool m_InitSuccess = false;
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
        // https://tldp.org/LDP/abs/html/exitcodes.html#EXITCODESREF
        FailedEngineLoopInit = 0x101,
        ExceptionThrown = 0x201,
        FailedMemorySubsystemInit = 0x301
    };

    inline EngineLoopNotification TranslateToEngineNotification(const IException& exception)
    {
        // There are no direct ways of checking for an exception's type, unless you want
        // to dynamic_cast<> and check..
        if (std::strcmp(exception.GetName(), "BadAllocException"))
            return EngineLoopNotification::OutOfMemory;

        return EngineLoopNotification::Crash;
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

    int UnguardedEngineMain(int argc, char** argv)
    {
        MemorySubsystemGuard memoryGuard{};
        if (!memoryGuard.Success())
            return ExitCode::FailedMemorySubsystemInit;

        // The mutex that makes sure only one exception's data gets recorded.
        ScopedPtr<Mutex> exceptionMutex = MakeScoped<Mutex>();
        g_ExceptionMutex = &exceptionMutex;

        int exitCode = ExitCode::Success;
        ScopedPtr<IEngineLoop> engineLoop = MakeScoped<DefaultEngineLoop>();

        // There are two try/catch blocks, the one here and the one
        // in EngineMain(). This one is for catching exceptions in application code,
        // which are still somewhat saveable.
        try
        {
            if (!engineLoop->Initialize(argc, argv))
                return ExitCode::FailedEngineLoopInit;

            exitCode = engineLoop->Run();
            engineLoop->Shutdown();         // Shutdown() shouldn't throw...
        }
        catch (const std::exception& stdException)
        {
            // Print diagnostic information.
            const IException* exception = dynamic_cast<const IException*>(&stdException);
            if (exception != nullptr)
            {
                std::printf("Program crashed due to %s exception.\nDescription: %s\n",
                            exception->GetName(), exception->GetDescription());
            }
            else
            {
                std::printf("Engine crashed due to a std::exception being thrown.\n"
                            "Description: %s\n",
                            stdException.what());
            }

            PrintStackTrace();

            // Shutdown!!
            engineLoop->Notify(TranslateToEngineNotification(*exception));
            engineLoop->Shutdown();

            return ExitCode::ExceptionThrown;
        }

        return exitCode;
    }

    int EngineMain(int argc, char** argv)
    {
        // If anything outside of the inner try/catch fails, just
        // return to the platform-specific entry code.
        try
        {
            return UnguardedEngineMain(argc, argv);
        }
        catch (const std::exception&)
        {
            return ExitCode::ExceptionThrown;
        }
    }
}
