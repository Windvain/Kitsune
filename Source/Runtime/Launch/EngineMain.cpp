#include <cstdio>

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/IException.h"

#include "Foundation/Memory/Memory.h"

#include "ApplicationCore/Application.h"
#include "ApplicationCore/Environment.h"

namespace Kitsune
{
    int UnguardedEngineMain(int argc, char** argv)
    {
        /* Subsystem Initialization */
        Memory::InitializeExplicit();
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

        /* Subsystem Shutdown */
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
