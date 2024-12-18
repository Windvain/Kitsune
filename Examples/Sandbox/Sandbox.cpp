#include "ApplicationCore/Application.h"
#include <cstdlib>
#include <iostream>

#include "Foundation/Common/Types.h"
#include "Foundation/Memory/Memory.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
    }

    ~Sandbox() { /* ... */ }
};

Application* CreateApplication(const CommandLineArgs& args)
{
    ApplicationSpecs specs;
    return new Sandbox(specs);
}
