#pragma once

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Application/DisplayManager.h"
#include "Application/Windows/WindowsScreen.h"

namespace Kitsune
{
    class WindowsDisplayManager : public DisplayManager
    {
    public:
        WindowsDisplayManager();
        ~WindowsDisplayManager();

    public:
        void Update() override;

    public:
        void UpdateScreenList();
        void OnScreenEvent();

    private:
        Array<ScopedPtr<WindowsScreen>> m_Screens;
    };
}
