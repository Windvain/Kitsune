#include "Core/Application.h"
#include <gtest/gtest.h>

#include "Launch/EngineLoop.h"
#include "Foundation/Memory/Memory.h"

using namespace Kitsune;

class FoundationTests : public Application
{
public:
    FoundationTests(const ApplicationSpecifications& specs,
                    const CommandLineArguments& arguments)
        : Application(specs, arguments)
    {
        testing::InitGoogleTest();

        EngineLoop* engineLoop = EngineLoop::GetInstance();
        engineLoop->Exit(RUN_ALL_TESTS());
    }

    ~FoundationTests() override
    {
        std::cin.get();
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& arguments)
{
    ApplicationSpecifications specs;
    specs.Name = "FoundationTests";
    specs.Headless = true;

    return Memory::New<FoundationTests>(specs, arguments);
}
