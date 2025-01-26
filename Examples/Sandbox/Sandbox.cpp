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
        CommandLineArguments arguments = Environment::GetCommandLineArguments();
        Logging::LogFormat("argc: {0}, argv: 0x{1}", arguments.Count(), (void*)arguments.Data());

        for (const char* arg : arguments)
            Logging::Log(arg);

        Logging::Log("\n\n\n");
        Logging::Info("Some information!");
        Logging::Warn("Hey something isn't right here");
        Logging::Error("Something went wrong..?");
        Logging::Fatal("Okay something definitely went wrong, abort!");

        KITSUNE_ASSERT(false, "Assertion failed.");
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
