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

        /* Allocate the primary monitor, will be needed for window creation */
        m_PrimaryMonitor = m_PlatformImpl->AllocatePrimaryMonitor();

        /* Create the primary window */
        WindowProperties windowProps;
        windowProps.Title = specs.Name;
        windowProps.Position = specs.WindowPosition;
        windowProps.VideoMode = GetPrimaryMonitor()->GetCurrentVideoMode();

        if (specs.ViewportSize == Vector2<Uint32>(0, 0))    // Yeah we're not sizing it to [0, 0]
            windowProps.Size = { 640, 480 };
        else
            windowProps.Size = specs.ViewportSize;

        // Reroute calls to a no-op window implementation, no need
        // for repeated checks.
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
