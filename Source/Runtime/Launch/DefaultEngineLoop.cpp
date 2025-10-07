#include "Launch/DefaultEngineLoop.h"

namespace Kitsune
{
    DefaultEngineLoop* DefaultEngineLoop::s_Instance = nullptr;

    DefaultEngineLoop::DefaultEngineLoop()
        : m_Application(nullptr)
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "More than one instance of the default engine loop has been created.");

        s_Instance = this;
    }

    DefaultEngineLoop::~DefaultEngineLoop()
    {
        s_Instance = nullptr;
    }

    bool DefaultEngineLoop::Initialize(int argc, char** argv)
    {
        m_CommandLineArguments = CommandLineArguments(argc, argv);
        m_Application = CreateApplication(m_CommandLineArguments);

        if (m_Application == nullptr)
            return false;

        return true;
    }

    int DefaultEngineLoop::Run()
    {
        while (!m_ExitRequested)
        {
            PlatformUpdate();
            m_Application->OnUpdate();
        }

        return m_ExitCode;
    }

    void DefaultEngineLoop::Shutdown()
    {
        if (m_Application == nullptr)
            return;

        Memory::Delete(m_Application);
        m_Application = nullptr;
    }

    void DefaultEngineLoop::Notify(EngineLoopNotification notification)
    {
        KITSUNE_UNUSED(notification);
    }
}
