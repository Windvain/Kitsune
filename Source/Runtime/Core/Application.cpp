#include "Core/Application.h"
#include "Graphics/GraphicsInstance.h"

namespace Kitsune
{
    Application::Application(ApplicationSpecifications specifications,
                             CommandLineArguments arguments)
    {
        m_GraphicsInstance = GraphicsInstance::Initialize({
            .Backend = specifications.GraphicsBackend,
            .DebugName = specifications.Name,
            .DebugEnabled = specifications.DebugGraphics
        });

        m_DisplayManager = DisplayManager::Initialize({
            .DisplayServer = specifications.DisplayServer,
            .WindowClassName = "Kitsune Window",
            .VirtualDisplay = {
                .Size = { 1920, 1080 },
                .RefreshRate = 60,
                .Scaling = 1.0f
            }
        });

        m_Window = m_DisplayManager->CreateWindow(specifications.MainWindow);

        m_Specifications = Move(specifications);
        m_Arguments = Move(arguments);
    }

    Application::~Application()
    {
        m_Window->Close();

        DisplayManager::Shutdown();
        GraphicsInstance::Shutdown();
    }

    void Application::Update(float delta)
    {
        KITSUNE_UNUSED(delta);
        m_DisplayManager->Update();
    }
}
