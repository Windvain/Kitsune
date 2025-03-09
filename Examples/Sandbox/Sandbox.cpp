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
        SharedPtr<IMonitor> monitor = GetPrimaryMonitor();
        VideoMode vidMode = monitor->GetCurrentVideoMode();

        Logging::LogFormat("Res: [{0}, {1}]", vidMode.Resolution.x, vidMode.Resolution.y);
        Logging::LogFormat("BPP: {0}", vidMode.BitsPerPixel);
        Logging::LogFormat("Hz: {0}", vidMode.RefreshRate);

        Vector2<Uint32> size = GetPrimaryWindow()->GetSize();
        Logging::LogFormat("Window Size: [{0}, {1}]", size.x, size.y);
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
    specs.PositionHint = WindowPositionHint::ScreenCenter;
    specs.WindowState = WindowState::Minimized;

    return Memory::New<Sandbox>(specs);
}
