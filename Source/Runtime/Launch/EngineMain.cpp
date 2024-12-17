#include <cstdio>

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/IException.h"

#include "Launch/EngineLoop.h"

namespace Kitsune
{
    int UnguardedEngineMain(int argc, char** argv)
    {
        auto engineLoop = EngineLoop(argc, argv);
        while (!engineLoop.IsExitRequested())
        {
            engineLoop.Tick();
        }

        return 0;
    }

    int EngineMain(int argc, char** argv)
    {
        try
        {
            return UnguardedEngineMain(argc, argv);
        }
        catch (const IException& exception)
        {
            std::printf(
                "An IException has been thrown. (Name: %s)\nDescription: %s",
                exception.GetName(), exception.GetDescription()
            );

            return 1;
        }

        KITSUNE_UNREACHABLE();
    }
}
