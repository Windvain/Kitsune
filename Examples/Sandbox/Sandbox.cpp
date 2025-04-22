#include "ApplicationCore/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Diagnostics/StackTrace.h"
#include "Foundation/Logging/GlobalLog.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
    }

    ~Sandbox()
    {
    }

public:
    void OnWindowResize(const Vector2<Uint32>& size)
    {
        IWindow* window = GetWindow();
        KITSUNE_TRACE_FORMAT("0x{0}: [{1}, {2}]", window, size.x, size.y);
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "你好，世界";
    specs.WindowState = WindowState::Fullscreen;
    // specs.Headless = true;
    specs.WindowResizable = false;
    specs.WindowPositionHint = WindowPositionHint::UsePosition;
    specs.WindowPosition = { 240, 480 };
    specs.ViewportSize = { 640, 422 };

    return Memory::New<Sandbox>(specs);
}
