#include "ApplicationCore/Application.h"
#include <gtest/gtest.h>

using namespace Kitsune;

class FoundationTests : public Application
{
public:
    FoundationTests(const ApplicationSpecs& specs)
        : Application(specs)
    {
    }

    ~FoundationTests() { /* ... */ }

public:
    void OnStart() override
    {
        testing::InitGoogleTest();
        Exit(RUN_ALL_TESTS());
    }

    void OnExit() override
    {
        std::cin.get();
    }
};

Application* CreateApplication(const CommandLineArgs& /* args */)
{
    ApplicationSpecs specs;
    return new FoundationTests(specs);
}
