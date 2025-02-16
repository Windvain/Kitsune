#include "ApplicationCore/Application.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecs& specs)
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "An application has already been instanced.");

        s_Instance = this;

        m_ApplicationSpecs = specs;
        m_PlatformImpl = IPlatformApplication::CreateApplicationImpl();
    }

    Application::~Application()
    {
        s_Instance = nullptr;
    }

    void Application::Update()
    {
        OnUpdate();
    }
}
