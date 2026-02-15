#include "Application/DisplayManager.h"
#include "Foundation/Diagnostics/Assert.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Application/Windows/WindowsDisplayManager.h"
#endif

namespace Kitsune
{
    DisplayManager* DisplayManager::s_Instance = nullptr;

    DisplayManager::DisplayManager()
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "An instance of the display manager had already been created.");

        s_Instance = this;
    }

    DisplayManager::~DisplayManager()
    {
        s_Instance = nullptr;
    }
}
