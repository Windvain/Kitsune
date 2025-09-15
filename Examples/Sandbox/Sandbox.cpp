#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Diagnostics/StackTrace.h"

#include <iostream>

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        KITSUNE_TRACE_FORMAT("Hello, {0:#0}", (short)3);
    }

    ~Sandbox()
    {
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "Sandbox";
    specs.ViewportSize = { 640, 480 };
    specs.WindowPositionHint = WindowPositionHint::ScreenCenter;

    return Memory::New<Sandbox>(specs);
}
