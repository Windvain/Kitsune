#include "ApplicationCore/Application.h"
#include "Foundation/Diagnostics/Assert.h"

#include "ApplicationCore/Null/NullWindow.h"

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

        /* Create the primary window */
        WindowProperties windowProps;
        windowProps.Title = specs.Name;
        windowProps.Position = specs.WindowPosition;

        if (specs.ViewportSize == Vector2<Uint32>(0, 0))    // Yeah we're not sizing it to [0, 0]
            windowProps.Size = { 640, 480 };
        else
            windowProps.Size = specs.ViewportSize;

        // Console apps don't have windows, but it would be stupid to try
        // and check whether the application is a console app every time an operation
        // needs to be done to the window.
        if (m_ApplicationSpecs.IsConsoleApp)
            m_PrimaryWindow = MakeNullWindow(windowProps);
        else
            m_PrimaryWindow = MakeWindow(windowProps);
    }

    Application::~Application()
    {
        s_Instance = nullptr;
    }

    void Application::Update()
    {
        m_PlatformImpl->PollEvents();
        OnUpdate();
    }
}
