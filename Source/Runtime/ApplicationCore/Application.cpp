#include "ApplicationCore/Application.h"

namespace Kitsune
{
    Application* Application::s_ApplicationInst = nullptr;

    Application::Application(const ApplicationSpecs& specs)
    {
        // TODO: assert(s_ApplicationInst == nullptr)
        s_ApplicationInst = this;

        m_ExitRequested = false;
        KITSUNE_UNUSED(specs);
    }

    Application::~Application()
    {
        s_ApplicationInst = nullptr;
    }
}
