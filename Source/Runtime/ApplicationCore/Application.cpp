#include "ApplicationCore/Application.h"
#include "Foundation/Diagnostics/Assert.h"

#include "ApplicationCore/Null/NullWindow.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    Application* Application::s_Instance = nullptr;

    Application::Application(ApplicationSpecs specs)
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "An application has already been instanced.");

        s_Instance = this;

        /* Filter out invalid arguments passed by the user */
        if (specs.ViewportSize == Vector2<Uint32>())
            throw InvalidArgumentException();

        m_ApplicationSpecs = Move(specs);

        /* Allocate the primary monitor, will be needed for window creation */
        m_PlatformImpl = IPlatformApplication::CreateApplicationImpl();
        m_PrimaryMonitor = m_PlatformImpl->AllocatePrimaryMonitor();

        /* Create the primary window */
        WindowProperties windowProps;
        windowProps.Title = m_ApplicationSpecs.Name;

        windowProps.Position = m_ApplicationSpecs.WindowPosition;
        windowProps.PositionHint = m_ApplicationSpecs.WindowPositionHint;
        windowProps.WindowState = m_ApplicationSpecs.WindowState;

        windowProps.VideoMode = GetPrimaryMonitor()->GetCurrentVideoMode();
        windowProps.Size = m_ApplicationSpecs.ViewportSize;

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
