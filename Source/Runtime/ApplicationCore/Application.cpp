#include "ApplicationCore/Application.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    Application* Application::s_ApplicationInst = nullptr;

    Application::Application(const ApplicationSpecs& specs)
    {
        KITSUNE_ASSERT(s_ApplicationInst == nullptr,
                       "An application has already been instanced.");

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
