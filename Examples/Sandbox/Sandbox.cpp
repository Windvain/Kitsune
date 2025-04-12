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
        // Array<int> arr = { 2 };
        // arr[5];

        StackTrace stackTrace = MakeStackTrace(0, 1024);
        KITSUNE_TRACE_FORMAT("Calling thread: {0}", stackTrace.GetCallingThreadName());

        for (auto& stackFrame : stackTrace)
        {
            KITSUNE_TRACE_FORMAT("0x{0}: {1} ({2}:{3})",
                stackFrame.GetFunctionAddress(), stackFrame.GetFunctionName(),
                stackFrame.GetFileName(), stackFrame.GetLineNumber());
        }
    }

    ~Sandbox()
    {
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
