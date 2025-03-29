#include "ApplicationCore/Application.h"
#include "Foundation/Logging/Logging.h"
#include "ApplicationCore/IWindow.h"

#include "Foundation/String/FormatArguments.h"

#include <iostream>

class X
{
public:

};

namespace Kitsune
{
    template<>
    class Formatter<X>
    {
    public:
        void Parse(const ParseContext&) {}

        template<typename It>
        It Format(const FormatContext<X, It>& context)
        {
            StringView str = "<CUSTOM>";
            return Algorithms::Copy(str.GetBegin(), str.GetEnd(), context.GetOutput());
        }
    };
}
using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        String str = Format("{0}", X());
        std::cout << str.Raw();

//         Logging::LogFormat("{{ {0},  {1} }}", 2, 2);
    }

    ~Sandbox()
    {
    }

public:
    void OnWindowResize(IWindow& /* window */, const Vector2<Uint32>& size) override
    {
        Logging::LogFormat("Size: [{0}, {1}]", size.x, size.y);
    }

    void OnWindowMove(IWindow& /* window */, const Vector2<Int32>& pos) override
    {
        Logging::LogFormat("Pos: [{0}, {1}]", pos.x, pos.y);
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "你好，世界";

    return Memory::New<Sandbox>(specs);
}
