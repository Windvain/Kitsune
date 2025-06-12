#include "Application/Application.h"
#include "ApplicationCore/WindowException.h"

#include "ApplicationCore/Null/NullMonitor.h"
#include "ApplicationCore/Null/NullWindow.h"

#include "Foundation/Utilities/MemberFunction.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    Application::Application(const ApplicationSpecs& specs)
    {
        VerifyApplicationSpecs(specs);
        m_ApplicationSpecs = specs;

        /* Retrieve all of the connected monitors */
        if (!specs.Headless)
            m_Monitors = RetrieveMonitors();
        else
            m_Monitors.PushBack(MakeShared<NullMonitor>());

        /* Create primary window */
        WindowProperties windowProps;
        windowProps.Size = specs.ViewportSize;
        windowProps.Title = specs.Name;
        windowProps.State = specs.WindowState;
        windowProps.Flags = WindowFlag::None;

        if (specs.WindowResizable)
            windowProps.Flags |= WindowFlag::Resizable;

        if (specs.WindowPositionHint == WindowPositionHint::UsePosition)
            windowProps.Position = specs.WindowPosition;
        else
        {
            SharedPtr<IMonitor> monitor = (specs.WindowPositionHint == WindowPositionHint::ScreenCenter) ?
                GetMonitor(specs.WindowMonitorIndex) : GetPrimaryMonitor();

            windowProps.Position = (monitor->GetVideoMode().Resolution - windowProps.Size) / 2;
        }

        if (specs.Headless)
            m_PrimaryWindow = MakeShared<NullWindow>(windowProps);
        else
        {
            m_PrimaryWindow = MakePlatformWindow(windowProps);
        }

        // Set window callbacks.
        m_PrimaryWindow->SetResizeCallback(MemberFunction(*this, &Application::OnWindowResize));
        m_PrimaryWindow->SetMoveCallback(MemberFunction(*this, &Application::OnWindowMove));

        m_PrimaryWindow->SetMinimizeCallback(MemberFunction(*this, &Application::OnWindowMinimize));
        m_PrimaryWindow->SetMaximizeCallback(MemberFunction(*this, &Application::OnWindowMaximize));
    }

    Application::~Application()
    {
    }

    void Application::Update()
    {
        PlatformUpdate();
        OnUpdate();
    }

    void Application::VerifyApplicationSpecs(const ApplicationSpecs& specs)
    {
        if (specs.ViewportSize == Vector2<Uint32>())
            throw InvalidArgumentException("Viewport size must not be [0, 0].");
    }

    void Application::VerifyWindowProperties(const WindowProperties& windowProps)
    {
        if (windowProps.Size == Vector2<Uint32>())
            throw InvalidArgumentException("Cannot create a window with a size of [0, 0].");
    }
}
