#include "Application/Application.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Diagnostics/LogicException.h"

namespace Kitsune
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecifications& specs)
        : m_Name(specs.Name),
          m_Description(specs.Description),
          m_Version(specs.Version)
    {
        if (s_Instance)
        {
            throw LogicException(
                "An application instance has already been created.");
        }

        WindowSpecifications windowSpecs;
        windowSpecs.Size = specs.ViewportSize;
        windowSpecs.Position = specs.WindowPosition;
        windowSpecs.Title = specs.Name;

        windowSpecs.Mode = specs.WindowMode;
        windowSpecs.Flags = specs.WindowFlags;

        DisplayManagerSpecifications displayManagerSpecs;
        displayManagerSpecs.Headless = specs.Headless;
        displayManagerSpecs.PrimaryWindowSpecs = windowSpecs;
        displayManagerSpecs.Backend = RenderingBackend::Vulkan;

        m_DisplayManager = DisplayManager::Initialize(displayManagerSpecs);
        s_Instance = this;

        KITSUNE_ENGINE_INFO(
            Application,
            "All subsystems have been initialized. Running user code...");
    }

    Application::~Application()
    {
        DisplayManager::Shutdown();
        m_DisplayManager = nullptr;
    }

    void Application::Update()
    {
        m_DisplayManager->Update();
        OnUpdate();
    }
}
