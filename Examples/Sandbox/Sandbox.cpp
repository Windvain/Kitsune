#include "ApplicationCore/Application.h"
#include <Windows.h>

#include "Foundation/Logging/Logging.h"
#include "ApplicationCore/IWindow.h"

#include "ApplicationCore/Null/NullWindow.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        auto window = GetPrimaryWindow();
        Vector2<Uint32> size = window->GetSize();
        Vector2<Int32> pos = window->GetPosition();     // TODO: Doesn't work

        window->SetSize({275, 775});
        size = window->GetSize();

        window->SetPosition({ 33, 42 });
        pos = window->GetPosition();

        AABB2<Int32> frameBB = window->GetFrameBoundingBox();
        AABB2<Int32> clientBB = window->GetBoundingBox();

        window->SetBoundingBox({{ 54, 32 }, { 444, 333 }});
        clientBB = window->GetBoundingBox();

        String title = window->GetTitle();
        window->SetTitle("大大大大大");
        title = window->GetTitle();

        WindowState state = window->GetState();
        window->SetState(WindowState::Maximized);
        size = window->GetSize();
        pos = window->GetPosition();

        state = window->GetState();

        window->SetState(WindowState::Minimized);
        size = window->GetSize();
        pos = window->GetPosition();

        state = window->GetState();

        window->SetState(WindowState::Floating);
        size = window->GetSize();
        pos = window->GetPosition();

        state = WindowState::Floating;

        /* No-op */
        window->GetPosition();
    }

    ~Sandbox()
    {
    }

public:
    void OnUpdate() override
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

    void OnWindowMaximize(IWindow& /* window */) override
    {
        Logging::Log("Maximized");
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecs specs;
    specs.Name = "你好，世界";
    specs.ViewportSize = { 640, 480 };
    specs.WindowPosition = { 120, 420 };
    specs.IsConsoleApp = true;

    return Memory::New<Sandbox>(specs);
}
