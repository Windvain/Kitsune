#include "ApplicationCore/CoreApplication.h"

namespace Kitsune
{
    CoreApplication* CoreApplication::s_Instance = nullptr;

    CoreApplication::CoreApplication()
    {
        KITSUNE_ASSERT(s_Instance == nullptr, "CoreApplication has already been instanced.");
        s_Instance = this;
    }

    CoreApplication::~CoreApplication()
    {
        s_Instance = nullptr;
    }
}
