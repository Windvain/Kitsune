#include "ApplicationCore/Application.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        /* ... */
    }

    ~Sandbox() { /* ... */ }
};

Application* CreateApplication(const CommandLineArgs& args)
{
    ApplicationSpecs specs;
    return new Sandbox(specs);
}
