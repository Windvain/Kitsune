#include "ApplicationCore/Application.h"

#include <iostream>
#include "Foundation/String/Format.h"

using namespace Kitsune;

class Base { /* ... */ };

namespace Kitsune
{
    template<>
    class Formatter<Base, char>
    {
    public:
        void Parse(...) { /* ... */ }
        String Format(Base base)
        {
            return "[UNKNOWN]";
        }
    };
}

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
        String formatted = Format("Hello, }} {{}}");
        std::cout << formatted.Raw();
    }
};

Application* CreateApplication(const CommandLineArgs& args)
{
    ApplicationSpecs specs;
    return Memory::New<Sandbox>(specs);
}
