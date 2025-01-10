#include "ApplicationCore/Application.h"
#include "Foundation/String/String.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
    }

    ~Sandbox() { /* ... */ }

public:
    void OnStart() override
    {
        String str = "+╪½╗êΦï";
        String move = Move(str);
    }

};

Application* CreateApplication(const CommandLineArgs& args)
{
    ApplicationSpecs specs;
    return Memory::New<Sandbox>(specs);
}
