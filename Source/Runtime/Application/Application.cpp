#include "Application/Application.h"
#include "Foundation/Logging/GlobalLog.h"

namespace Kitsune
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecifications& specs)
        : m_Name(specs.Name), m_Description(specs.Description),
          m_Version(specs.Version)
    {
        if (s_Instance)
        {
            KITSUNE_ENGINE_ERROR_("An application has already been instanced.");
            return;
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

        KITSUNE_ENGINE_INFO_FORMAT_(
            "Application \"{0}\" has been fully initialized. Running user code...",
            m_Name);
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
