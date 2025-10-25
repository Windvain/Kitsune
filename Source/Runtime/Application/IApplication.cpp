#include "Application/IApplication.h"

#include "Launch/DefaultEngineLoop.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    IApplication::IApplication(const ApplicationSpecifications& specs)
        : m_ApplicationSpecs(specs)
    {
        auto* engineLoop = DefaultEngineLoop::GetInstance();
        KITSUNE_ASSERT(engineLoop != nullptr,
                       "The engine loop has not been created.");

        KITSUNE_ASSERT(engineLoop->GetApplication() == nullptr,
                       "An instance of the application has already been created.");
    }
}
