#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Utilities/Function.h"

#include <thread>
#include "Foundation/Diagnostics/StackTrace.h"

using namespace Kitsune;

void MyFunction()
{
    StackTrace stackTrace = MakeStackTrace();
    for (auto& frame : stackTrace)
    {
        String filename = frame.GetFileName();
        String funcname = frame.GetFunctionName();
        Uint64 line = frame.GetLineNumber();

        std::printf("[x]: %s [%s:%llu]\n", funcname.Raw(), filename.Raw(), line);
    }
}

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        std::thread thread(MyFunction);
        thread.join();
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
    specs.WindowPositionHint = WindowPositionHint::PrimaryScreenCenter;
    specs.ViewportSize = { 640, 422 };

    return Memory::New<Sandbox>(specs);
}
