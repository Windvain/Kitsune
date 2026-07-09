#include "Core/Application.h"
#include "Launch/EngineLoop.h"

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
        EngineLoop::GetInstance()->Exit(1);
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& arguments)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";

    return Memory::New<Sandbox>(specs, arguments);
}
