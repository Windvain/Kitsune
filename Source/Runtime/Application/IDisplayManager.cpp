#include "Application/IDisplayManager.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    IDisplayManager* IDisplayManager::s_Instance = nullptr;

    IDisplayManager::IDisplayManager()
    {
        KITSUNE_ASSERT(s_Instance == nullptr,
                       "An instance of the display manager had already been created.");

        s_Instance = this;
    }

    IDisplayManager::~IDisplayManager()
    {
        s_Instance = nullptr;
    }
}
