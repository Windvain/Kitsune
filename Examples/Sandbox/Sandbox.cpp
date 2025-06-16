#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Filesystem/Path.h"

#include "Foundation/String/UnicodeConversion.h"
#include "Foundation/Iterators/BackInsertIterator.h"

using namespace Kitsune;

class Sandbox : public Application
{
public:
    Sandbox(const ApplicationSpecs& specs)
        : Application(specs)
    {
        Filesystem::Path path(L"\\\\.\\Volume{b75e2c83-0000-0000-0000-602f00000000}");
        path = path.GetAnchorPath();

        String str;
        Unicode::Convert(path.Native().GetBegin(), path.Native().GetEnd(), BackInsertIterator(str));

        KITSUNE_LOG(str);
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
