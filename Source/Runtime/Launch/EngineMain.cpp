#include <cstdio>
#include <cinttypes>

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/IException.h"

#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Logging/AnsiColorSink.h"

#include "Foundation/Threading/ThisThread.h"

#include "ApplicationCore/Application.h"
#include "ApplicationCore/Environment.h"

namespace Kitsune
{
    class MemorySubsystemGuard
    {
    public:
        MemorySubsystemGuard() { Memory::InitializeExplicit(); }
        ~MemorySubsystemGuard()
        {
            Memory::Shutdown();
        }
    };

    int UnguardedEngineMain(int argc, char** argv)
    {
        /* Foundation Subsystems Initialization */
        // Make sure the memory subsystem gets shutdown last, there
        // might be stack variables in the initialization code which
        // could attempt to free memory **after** Memory::Shutdown().
        MemorySubsystemGuard memoryGuard{};

        // Initialize the global logger.
        ScopedPtr<Logger> globalLogger = MakeScoped<Logger>("GLOBAL");
        SetGlobalLogger(globalLogger.Get());

        // Create streams to stdout and stderr, if they exist.
#if !defined(KITSUNE_BUILD_RELEASE)
        ConsoleOutputStream consoleOutStream(/* Error stream: */ false);
        ConsoleOutputStream consoleErrorStream(/* Error stream: */ true);

        globalLogger->GetSinks().PushBack(
            MakeShared<AnsiColorSink>(consoleOutStream, consoleErrorStream));
#endif

        /* Remaining Subsystems Initialization */
        Environment::Initialize(argc, argv);

        /* App Startup */
        // This is where client code gets called for the first time.
        Application* app = Kitsune::CreateApplication(Environment::GetCommandLineArguments());     // << Here..
        while (!app->IsExitRequested())
            app->Update();

        int exitCode = app->GetExitCode();
        Memory::Delete(app);

        /* Remaining Subsystems Shutdown */
        Environment::Shutdown();

        /* Foundation Subsystems Shutdown */
        // Just in case, who knows what might happen..
        SetGlobalLogger(nullptr);

        return exitCode;
    }

    void PrintStackTrace()
    {
        StackTrace* stackTrace = ThisThread::GetExceptionStackTrace();
        if (stackTrace == nullptr)
        {
            std::printf("No stack traces were created.\n");
            return;
        }

        std::printf("Stack trace: \nThread Name: %s\n",
                    stackTrace->GetCallingThreadName().Raw());

        Uint32 index = 0;
        for (auto& frame : *stackTrace)
        {
             std::printf("#%" PRIu32 ": (0x%p) %s\n\tfrom %s:%" PRIu32 "\n",
                         index, frame.GetFunctionAddress(), frame.GetFunctionName().Raw(),
                         frame.GetFileName().Raw(), frame.GetLineNumber());

            ++index;
        }
    }

    int EngineMain(int argc, char** argv)
    {
        // The try/catch makes it harder to debug, just add it in when compiling release builds.
#if defined(KITSUNE_BUILD_RELEASE)
        try
#endif
        {
            return UnguardedEngineMain(argc, argv);
        }
#if defined(KITSUNE_BUILD_RELEASE)
        catch (const IException& exception)
        {
            // Can't use Logger API here.. Just output whatever we can
            // to the console instead.
            std::printf(
                "An IException has been thrown. (Name: %s)\nDescription: %s\n",
                exception.GetName(), exception.GetDescription()
            );

            PrintStackTrace();
            return 1;
        }
#endif

        // Tell the compiler to shush.
        KITSUNE_UNREACHABLE();
    }
}
