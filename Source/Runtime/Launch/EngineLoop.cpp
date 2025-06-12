#include "Launch/EngineLoop.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Logging/AnsiColorSink.h"

#include "Application/Application.h"
#include "ApplicationCore/Environment.h"

namespace Kitsune
{
    EngineLoop::EngineLoop(int argc, char** argv)
    {
        PreInitialize();
        Initialize(argc, argv);
    }

    EngineLoop::~EngineLoop()
    {
        if (m_IsFullyInitialized)
            PreShutdown();

        if (m_IsPreInitialized)
            Shutdown();
    }

    void EngineLoop::PreInitialize()
    {
        m_GlobalLogger = MakeScoped<Logger>("GLOBAL");
        SetGlobalLogger(m_GlobalLogger.Get());

#if !defined(KITSUNE_BUILD_RELEASE)
            m_GlobalLogger->GetSinks().PushBack(
                MakeShared<AnsiColorSink>(MakeShared<ConsoleOutputStream>()));
#endif

        m_IsPreInitialized = true;
    }

    void EngineLoop::Initialize(int argc, char** argv)
    {
        Environment::Initialize(argc, argv);
        m_IsFullyInitialized = true;
    }

    void EngineLoop::Run()
    {
        m_Application = Kitsune::CreateApplication(Environment::GetCommandLineArguments());
        while (!m_Application->IsExitRequested())
        {
            m_Application->Update();
        }
    }

    int EngineLoop::PreShutdown()
    {
        KITSUNE_ASSERT(m_IsFullyInitialized, "PreShutdown() should not be called when the engine has"
                                             "not been fully initialized.");

        int exitCode = m_Application->GetExitCode();
        Memory::Delete(m_Application);

        Environment::Shutdown();
        m_IsFullyInitialized = false;

        return exitCode;
    }

    void EngineLoop::Shutdown()
    {
        KITSUNE_ASSERT(m_IsPreInitialized, "Shutdown() should not be called when the engine has"
                                           "not been pre-initialized.");

        if (GetGlobalLogger() == m_GlobalLogger.Get())
            SetGlobalLogger(nullptr);

        m_IsPreInitialized = false;
    }
}
