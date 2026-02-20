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

        DisplayManagerSpecifications displayManagerSpecs;
        displayManagerSpecs.Headless = specs.Headless;

        m_DisplayManager = DisplayManager::Initialize(displayManagerSpecs);
        s_Instance = this;
    }

    Application::~Application()
    {
        DisplayManager::Shutdown();
    }

    void Application::Update()
    {
        m_DisplayManager->Update();
        OnUpdate();
    }
}
