#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Diagnostics/StackTrace.h"
#include "Foundation/Logging/GlobalLog.h"

#include "Foundation/Utilities/Function.h"
#include "Foundation/Utilities/MemberFunction.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        Function<int(float)> func = [&](float) -> int { return 2; };
        KITSUNE_TRACE_FORMAT("{0}", func(3.0f));
    }

    ~Sandbox()
    {
    }

public:
    void OnWindowResize(const Vector2<Uint32>& size)
    {
        SharedPtr<IWindow> window = GetWindow();
        KITSUNE_TRACE_FORMAT("0x{0}: [{1}, {2}]", window.Get(), size.x, size.y);
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "你好，世界";
    specs.WindowState = WindowState::Windowed;
    // specs.Headless = true;
    // specs.WindowResizable = false;
    specs.WindowPositionHint = WindowPositionHint::UsePosition;
    specs.WindowPosition = { 240, 480 };
    specs.ViewportSize = { 640, 422 };

    return Memory::New<Sandbox>(specs);
}
