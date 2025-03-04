#include "ApplicationCore/Application.h"
#include "Foundation/Logging/Logging.h"
#include "ApplicationCore/IWindow.h"

#include "ApplicationCore/Environment.h"
#include "Foundation/Filesystem/Path.h"

#include <iostream>

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        Path path = Environment::GetExecutablePath();
        std::wcout << std::wstring(path.Native().Raw(), path.Native().Size());
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
