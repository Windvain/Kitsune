#include "ApplicationCore/Application.h"

#include "Foundation/Logging/Logging.h"
#include "ApplicationCore/Environment.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        Logging::Log("Hello, World!");
        Exit(2);
    }

    ~Sandbox()
    {
    }

public:
    void Update()
    {
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    return Memory::New<Sandbox>(specs);
}
