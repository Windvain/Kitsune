#include "Core/Application.h"

#include "Foundation/Memory/Memory.h"
#include "Foundation/Logging/Logger.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecifications& specs,
            const CommandLineArguments& arguments)
        : Application(specs, arguments)
    {
        KITSUNE_TRACE("Hello, World!");
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& arguments)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";

    specs.DisplayServer = "Windows";
    specs.MainWindow = {
        .Title = "Sandbox",
        .Size = { 640, 480 },
        .Flags = WindowCreationFlags::None,
        .State = WindowState::Maximized
    };

    specs.GraphicsBackend = "Vulkan";
    specs.DebugGraphics = true;

    return Memory::New<Sandbox>(specs, arguments);
}
