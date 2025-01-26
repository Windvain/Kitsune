#include <cstdio>

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/IException.h"

#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/Logging.h"
#include "Foundation/Logging/ConsoleLoggerSink.h"

#include "ApplicationCore/Application.h"
#include "ApplicationCore/Environment.h"

namespace Kitsune
{
    int UnguardedEngineMain(int argc, char** argv)
    {
        /* Foundation Subsystems Initialization */
        Memory::InitializeExplicit();

        // Initialize the global logger.
        Logger* globalLogger = Memory::New<Logger>("GLOBAL", MakeShared<ConsoleLoggerSink>());
        globalLogger->SetFlushSeverity(LogSeverity::Error);
        globalLogger->SetMinimumSeverity(LogSeverity::Trace);

        Logging::SetGlobalLogger(globalLogger);

        /* Remaining Subsystems Initialization */
        Environment::Initialize(argc, argv);

        /* App Startup */
        // This is where client code gets called for the first time.
        Application* app = Kitsune::CreateApplication(Environment::GetCommandLineArguments());     // << Here..
        while (!app->IsExitRequested())
        {
            app->Update();
        }

        // Applications *should* be created via the Memory::New<T>() function.
        Memory::Delete(app);

        /* Remaining Subsystems Shutdown */
        Environment::Shutdown();

        /* Foundation Subsystems Shutdown */
        Memory::Delete(Logging::GetGlobalLogger());
        Logging::SetGlobalLogger(nullptr);

        Memory::Shutdown();
        return 0;
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
            std::printf(
                "An IException has been thrown. (Name: %s)\nDescription: %s",
                exception.GetName(), exception.GetDescription()
            );

            return 1;
        }
#endif

        // Tell the compiler to shush.
        KITSUNE_UNREACHABLE();
    }
}
