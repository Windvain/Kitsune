#include "Core/Application.h"
#include "Foundation/Common/Macros.h"

namespace Kitsune
{
    Application::Application(const ApplicationSpecifications& specs,
                             const CommandLineArguments& arguments)
    {
        KITSUNE_UNUSED(specs);
        KITSUNE_UNUSED(arguments);
    }

    Application::~Application()
    {
        (void)0;
    }
}
