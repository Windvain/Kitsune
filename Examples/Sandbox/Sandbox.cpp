#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/Logger.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecifications& specs)
        : Application(specs)
    {
        KITSUNE_TRACE("Hello, World!");
    }

    ~Sandbox()
    {
    }

    void OnUpdate()
    {
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";

    return Memory::New<Sandbox>(specs);
}
