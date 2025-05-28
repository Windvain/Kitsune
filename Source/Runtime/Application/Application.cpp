#include "Application/Application.h"

#include "ApplicationCore/Null/NullMonitor.h"
#include "ApplicationCore/Null/NullWindow.h"

#include "Foundation/Utilities/MemberFunction.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecs& specs)
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "An application has already been instanced.");

        VerifyApplicationSpecs(specs);
        m_ApplicationSpecs = specs;

        s_Instance = this;

        /* Retrieve all of the connected monitors */
        if (!specs.Headless)
            m_Monitors = RetrieveAllMonitors();
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
            m_PrimaryWindow = MakeWindow(windowProps);

        // Set window callbacks.
        m_PrimaryWindow->SetResizeCallback(MemberFunction(*this, &Application::OnWindowResize));
        m_PrimaryWindow->SetMoveCallback(MemberFunction(*this, &Application::OnWindowMove));

        m_PrimaryWindow->SetMinimizeCallback(MemberFunction(*this, &Application::OnWindowMinimize));
        m_PrimaryWindow->SetMaximizeCallback(MemberFunction(*this, &Application::OnWindowMaximize));
    }

    Application::~Application()
    {
        s_Instance = nullptr;
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
}
