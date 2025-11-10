#include "Application/IApplication.h"

#include "Foundation/Memory/Memory.h"
#include "Application/IDisplayManager.h"

#include "Foundation/Logging/GlobalLog.h"

using namespace Kitsune;

class Sandbox : public IApplication
{
public:
    Sandbox(const ApplicationSpecifications& specs)
        : IApplication(specs)
    {
        IDisplayManager* displayManager = IDisplayManager::GetInstance();

        WindowSpecifications wndSpecs;
        wndSpecs.Position = { 120, 200 };
        wndSpecs.Size = { 640, 480 };
        wndSpecs.Title = "Hello! 😱😂😁😘";

        wndSpecs.PositionHint = WindowPositionHint::PrimaryScreenCenter;
        displayManager->RegisterWindow(wndSpecs);
    }

    ~Sandbox()
    {
    }

    void OnViewportResize(const Vector2<Uint32>& size) override
    {
        KITSUNE_INFO_FORMAT("Window resized! New size: ({0}, {1})", size.x, size.y);
    }
};

IApplication* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";

    return Memory::New<Sandbox>(specs);
}
