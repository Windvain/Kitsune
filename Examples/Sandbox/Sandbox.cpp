#include "ApplicationCore/Application.h"
#include <Windows.h>

#include "Foundation/Logging/Logger.h"
#include "Foundation/Logging/Logging.h"
#include "ApplicationCore/Environment.h"
#include "Foundation/Logging/ConsoleLoggerSink.h"

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
