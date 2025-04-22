#include "ApplicationCore/Application.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecs& specs)
        : CoreApplication(specs)
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "An application has already been instanced.");

        VerifyApplicationSpecs(specs);
        s_Instance = this;

        /* Create primary window */
        WindowProperties windowProps;
        windowProps.Size = specs.ViewportSize;
        windowProps.Title = specs.Name;
        windowProps.State = specs.WindowState;
        windowProps.Flags = specs.WindowResizable ? WindowFlag::Resizable : WindowFlag::None;

        if (specs.WindowPositionHint == WindowPositionHint::UsePosition)
            windowProps.Position = specs.WindowPosition;
        else
        {
            IMonitor* monitor = (specs.WindowPositionHint == WindowPositionHint::ScreenCenter) ?
                GetMonitor(specs.WindowMonitorIndex) : GetPrimaryMonitor();

            windowProps.Position = (monitor->GetVideoMode().Resolution - windowProps.Size) / 2;
        }

        CoreApplication::MakeWindow(windowProps);
    }

    Application::~Application()
    {
    }

    void Application::Update()
    {
        CoreApplication::PlatformUpdate();
        OnUpdate();
    }

    void Application::VerifyApplicationSpecs(const ApplicationSpecs& specs)
    {
        if (specs.ViewportSize == Vector2<Uint32>())
            throw InvalidArgumentException("Viewport size must not be [0, 0].");
    }
}
