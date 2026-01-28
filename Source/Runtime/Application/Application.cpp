#include "Application/Application.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecifications& specs)
        : m_Name(specs.Name), m_Description(specs.Description),
          m_Version(specs.Version)
    {
        if (s_Instance)
            return;

        s_Instance = this;
    }
}
