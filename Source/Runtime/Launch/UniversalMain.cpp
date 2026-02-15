#include "Foundation/Memory/Memory.h"
#include <cstdio>

#include "Launch/EngineLoop.h"
#include "Foundation/Diagnostics/Exception.h"

namespace Kitsune
{
    class MemorySubsystemGuard
    {
    public:
        inline MemorySubsystemGuard()
            : m_Initialized(Memory::InitializeExplicit())
        {
        }

        inline ~MemorySubsystemGuard()
        {
            if (m_Initialized)
                Memory::Shutdown();
        }

        inline bool IsInitialized() const
        {
            return m_Initialized;
        }

    private:
        bool m_Initialized;
    };

    static int UnguardedEngineMain(int argc, char** argv)
    {
        // The memory subsystem is going to be used a lot in engine initialization.
        MemorySubsystemGuard _initGuard{ /* ... */ };
        if (!_initGuard.IsInitialized())
            return 1;

        bool exceptionThrown = false;
        EngineLoop engineLoop;

        try
        {
            engineLoop.Initialize(argc, argv);
            engineLoop.Run();
        }
        catch (const std::exception& exception)
        {
            exceptionThrown = true;

            // Exceptions could be thrown by the standard library (std::exception) or by
            // game code (Kitsune::Exception).
            const auto* engineException = dynamic_cast<const Exception*>(&exception);
            if (engineException != nullptr)
            {
                std::printf("Program crashed due to %s exception.\nDescription: %s\n\n",
                            engineException->GetName(), engineException->GetDescription());
            }
            else
            {
                std::printf("Program crashed due to a std::exception (%s).\n\n",
                            exception.what());
            }

            String backtrace = Format("{0}", engineLoop.GetExceptionBacktrace());
            std::puts(backtrace.Raw());
        }

        int exitCode = engineLoop.Shutdown();
        if (exceptionThrown)
            return 1;

        return exitCode;
    }

    // Contains the code that every platform's entry calls into.
    // Basically serves as a guard for UnguardedEngineMain().
    int UniversalMain(int argc, char** argv)
    {
        // This is the try/catch block for the scope that prints out exception and
        // backtrace information.
        try
        {
            return UnguardedEngineMain(argc, argv);
        }
        catch (const std::exception&)
        {
            return 1;
        }
    }
}
