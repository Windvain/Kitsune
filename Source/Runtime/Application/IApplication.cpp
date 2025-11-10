#include "Application/IApplication.h"

#include "Launch/DefaultEngineLoop.h"
#include "Application/IDisplayManager.h"

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

        // Create the primary window.
        WindowSpecifications windowSpecs;
        windowSpecs.Title = specs.Name;
        windowSpecs.Size = specs.ViewportSize;
        windowSpecs.Flags = specs.WindowFlags;

        windowSpecs.Position = specs.WindowPosition;
        windowSpecs.PositionHint = specs.WindowPositionHint;

        IDisplayManager* displayManager = IDisplayManager::GetInstance();
        KITSUNE_UNUSED(displayManager->RegisterWindow(windowSpecs));
    }
}
