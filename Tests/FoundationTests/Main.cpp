#include "Application/Application.h"
#include <gtest/gtest.h>

// #include "Launch/DefaultEngineLoop.h"
#include "Foundation/Memory/Memory.h"

using namespace Kitsune;

class FoundationTests : public Application
{
public:
    FoundationTests(const ApplicationSpecifications& specs)
        : Application(specs)
    {
        testing::InitGoogleTest();

//        DefaultEngineLoop* engineLoop = DefaultEngineLoop::GetInstance();
//        engineLoop->Exit(RUN_ALL_TESTS());
    }

    ~FoundationTests()
    {
        std::cin.get();
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    return Memory::New<FoundationTests>(specs);
}
