#include "Launch/EngineLoop.h"

#include "Foundation/Logging/ConsoleLogSink.h"
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
        m_Logger = Memory::New<Logger>();

#if !defined(KITSUNE_BUILD_PRODUCTION)
        m_Logger->RegisterSink(MakeScoped<ConsoleLogSink>());
#endif
        KITSUNE_ENGINE_INFO_FORMAT(
            Launch,
            "Initializing Kitsune Engine {0}. "
            "For the source code, visit https://github.com/Windvain/Kitsune",
            GetEngineVersion());

        KITSUNE_ENGINE_INFO(
            Launch,
            "Kitsune Engine initialization step ran successfully. Calling the "
            "application's constructor.");

        m_Application = CreateApplication(m_CommandLineArguments);
        if (m_Application == nullptr)
            Exit(1);
    }

    // NOTE: This is done just to suppress warnings. Remove the NOLINT comment once
    // Run() is made non-const.
    // NOLINTNEXTLINE(readability-make-member-function-const)
    void EngineLoop::Run()
    {
        KITSUNE_ENGINE_INFO(
            Launch,
            "Running the application, application callbacks will start to be called "
            "from here on!");

        while (!m_ExitRequested)
        {
            m_Application->OnUpdate(/* Temp */ 0);
        }
    }

    int EngineLoop::Shutdown()
    {
        KITSUNE_ENGINE_INFO(Launch, "Kitsune Engine shutting down, goodbye!");

        if (m_Application != nullptr)
        {
            Memory::Delete(m_Application);
            m_Application = nullptr;
        }

        Memory::Delete(m_Logger);
        m_Logger = nullptr;

        return m_ExitCode;
    }
}
