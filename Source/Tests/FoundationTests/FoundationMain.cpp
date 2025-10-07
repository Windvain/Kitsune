#include "Application/IApplication.h"
#include "Launch/DefaultEngineLoop.h"

#include <gtest/gtest.h>
#include "Foundation/Memory/Memory.h"

using namespace Kitsune;

class FoundationTests : public IApplication
{
public:
    FoundationTests(const ApplicationSpecifications& specs)
        : IApplication(specs)
    {
        testing::InitGoogleTest();

        DefaultEngineLoop* engineLoop = DefaultEngineLoop::GetInstance();
        engineLoop->Exit(RUN_ALL_TESTS());
    }

    ~FoundationTests()
    {
        std::cin.get();
    }
};

IApplication* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    return Memory::New<FoundationTests>(specs);
}
