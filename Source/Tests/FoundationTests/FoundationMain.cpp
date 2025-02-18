#include "ApplicationCore/Application.h"
#include "Foundation/Memory/Memory.h"

#include <gtest/gtest.h>

using namespace Kitsune;

class FoundationTests : public Application
{
public:
    FoundationTests(const ApplicationSpecs& specs)
        : Application(specs)
    {
        testing::InitGoogleTest();
        Exit(RUN_ALL_TESTS());
    }

    ~FoundationTests()
    {
        std::cin.get();
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.IsConsoleApp = true;

    return Memory::New<FoundationTests>(specs);
}
