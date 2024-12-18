#include "ApplicationCore/Application.h"

namespace Kitsune
{
    Application* Application::s_ApplicationInst = nullptr;

    Application::Application(const ApplicationSpecs& specs)
    {
        // TODO: assert(s_ApplicationInst == nullptr)
        s_ApplicationInst = this;

        m_ExitRequested = false;
        KITSUNE_UNUSED(specs);
    }

    Application::~Application()
    {
        s_ApplicationInst = nullptr;
    }

    void Application::Exit(int exitCode)
    {
        m_ExitRequested = true;
        m_ExitCode = exitCode;

        ProcessExitRequest(false, exitCode);
    }

    void Application::ForceExit(int exitCode)
    {
        m_ExitRequested = true;
        m_ExitCode = exitCode;

        ProcessExitRequest(true, exitCode);
    }
}
