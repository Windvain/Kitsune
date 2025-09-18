#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/ConsoleLogger.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        LogPayload payload;
        payload.Location = SourceLocation::Current();
        payload.Severity = LogSeverity::Info;
        payload.LoggerName = "GLOBAL";
        payload.Message = "Hello, World!";

        ConsoleLogger logger;
        logger.Log(payload);
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
