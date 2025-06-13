#include <cstdio>
#include <cinttypes>

#include "Launch/EngineLoop.h"

#include "Foundation/Common/Macros.h"
#include "Foundation/Diagnostics/IException.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Threading/ThisThread.h"

namespace Kitsune
{
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

    int UnguardedEngineMain(int argc, char** argv)
    {
        // EngineLoop needs to allocate heap memory, initialize the memory
        // subsystem first.
        MemorySubsystemGuard memoryGuard{};

        EngineLoop engineLoop(argc, argv);
        engineLoop.Run();

        Application* app = engineLoop.GetApplicationInstance();
        return app->GetExitCode();
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
            String funcName = frame.GetFunctionName();
            String fileName = frame.GetFileName();

            std::printf("#%" PRIu32 ": (0x%p) %s\n\tfrom %s:%" PRIu32 "\n",
                         index, frame.GetFunctionAddress(), funcName.Raw(),
                         fileName.Raw(), frame.GetLineNumber());

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
