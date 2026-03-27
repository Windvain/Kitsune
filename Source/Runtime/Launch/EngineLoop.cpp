#include "Launch/EngineLoop.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Logging/ConsoleLogger.h"

#include "Foundation/Diagnostics/LogicException.h"

namespace Kitsune
{
    EngineLoop* EngineLoop::s_Instance = nullptr;

    EngineLoop::EngineLoop()
    {
        if (s_Instance != nullptr)
        {
            throw LogicException(
                "An instance of the engine loop has already been created.");
        }

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
        KITSUNE_ENGINE_INFO_FORMAT(
            Launch,
            "Initializing Kitsune Engine {0}. "
            "For the source code, visit https://github.com/Windvain/Kitsune",
            GetEngineVersion());

        m_Application = CreateApplication(m_CommandLineArguments);
        if (m_Application == nullptr)
            return;

        KITSUNE_ENGINE_INFO(
            Launch,
            "Kitsune Engine initialization step ran successfully.");
    }

    void EngineLoop::Run()
    {
        KITSUNE_ENGINE_INFO(
            Launch,
            "Running the application, application callbacks will start to be called "
            "from here on!");

        while (!m_ExitRequested)
        {
            m_Application->Update();
        }
    }

    int EngineLoop::Shutdown()
    {
        KITSUNE_ENGINE_INFO(Launch, "Kitsune Engine shutting down, goodbye!");

        if (m_Application != nullptr)
            Memory::Delete(m_Application);

        m_Loggers.Clear();
        return m_ExitCode;
    }
}
