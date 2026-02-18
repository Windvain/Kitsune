#include "Launch/EngineLoop.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Logging/ConsoleLogger.h"

namespace Kitsune
{
    EngineLoop* EngineLoop::s_Instance = nullptr;

    EngineLoop::EngineLoop()
    {
        if (s_Instance != nullptr)
            return;

        s_Instance = this;
    }

    EngineLoop::~EngineLoop()
    {
        s_Instance = nullptr;
    }

    void EngineLoop::Initialize(int argc, char** argv)
    {
        m_CommandLineArguments = CommandLineArguments(argc, argv);

#if !defined(KITSUNE_BUILD_PRODUCTION)
        RegisterLogger<ConsoleLogger>();
#endif

        KITSUNE_ENGINE_INFO_FORMAT_(
            "Initializing Kitsune Engine {0}. "
            "For the source code, visit https://github.com/Windvain/Kitsune",
            GetEngineVersion());

        DisplayManagerSpecifications displayManagerSpecs;
        m_DisplayManager = DisplayManager::Initialize(displayManagerSpecs);

        KITSUNE_ENGINE_INFO_FORMAT_(
            "Initializing application \"{0}\"",
            argv[0]);

        m_Application = CreateApplication(m_CommandLineArguments);
        if (m_Application == nullptr)
            return;

        KITSUNE_ENGINE_INFO_("Successfully initialized the engine loop.");
    }

    void EngineLoop::Run()
    {
        while (!m_ExitRequested)
        {
            m_DisplayManager->Update();
            m_Application->OnUpdate();
        }
    }

    int EngineLoop::Shutdown()
    {
        Memory::Delete(m_Application);
        DisplayManager::Shutdown();

        m_Loggers.Clear();
        return m_ExitCode;
    }
}
