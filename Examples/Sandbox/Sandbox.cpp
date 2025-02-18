#include "ApplicationCore/Application.h"
#include <Windows.h>

#include "Foundation/Logging/Logging.h"
#include "ApplicationCore/IWindow.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        SharedPtr<IWindow> window = GetPrimaryWindow();
        AABB2<Int32> rect = window->GetFrameBoundingBox();

        Logging::LogFormat("[{0}, {1}]", window->GetSize().x, window->GetSize().y);
        Logging::LogFormat("[[{0}, {1}], [{2}, {3}]]", rect.TopLeft.x, rect.TopLeft.y, rect.BottomRight.x, rect.BottomRight.y);
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
