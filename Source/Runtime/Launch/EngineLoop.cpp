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

        // Just in case I remove the condition that the application needs a minimum
        // of one argument.
        KITSUNE_ASSERT(m_CommandLineArguments.GetCount() > 0,
                       "Application should be supplied with at least one command "
                       "line argument.");

        m_Application = CreateApplication(m_CommandLineArguments);
        if (m_Application == nullptr)
            return;

        KITSUNE_ENGINE_INFO_("The application has been successfully initialized.");
    }

    void EngineLoop::Run()
    {
        while (!m_ExitRequested)
        {
            m_Application->Update();
        }
    }

    int EngineLoop::Shutdown()
    {
        Memory::Delete(m_Application);
        m_Loggers.Clear();

        return m_ExitCode;
    }
}
