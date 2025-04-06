#include "ApplicationCore/Application.h"
#include "Foundation/Memory/Memory.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
    }

    ~Sandbox()
    {
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "你好，世界";
    specs.ViewportSize = { 640, 480 };
    specs.WindowPosition = { 120, 420 };
    specs.IsConsoleApp = true;

    return Memory::New<Sandbox>(specs);
}
