#include "Application/IApplication.h"
#include "Foundation/Memory/Memory.h"

using namespace Kitsune;

class Sandbox : public IApplication
{
public:
    Sandbox(const ApplicationSpecifications& specs)
        : IApplication(specs)
    {
    }

    ~Sandbox()
    {
    }
};

IApplication* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";

    return Memory::New<Sandbox>(specs);
}
