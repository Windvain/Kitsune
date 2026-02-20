#include "Application/Application.h"
#include <gtest/gtest.h>

#include "Launch/EngineLoop.h"
#include "Foundation/Memory/Memory.h"

using namespace Kitsune;

class FoundationTests : public Application
{
public:
    FoundationTests(const ApplicationSpecifications& specs)
        : Application(specs)
    {
        testing::InitGoogleTest();

        EngineLoop* engineLoop = EngineLoop::GetInstance();
        engineLoop->Exit(RUN_ALL_TESTS());
    }

    ~FoundationTests()
    {
        std::cin.get();
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "FoundationTests";
    specs.Headless = true;

    return Memory::New<FoundationTests>(specs);
}
