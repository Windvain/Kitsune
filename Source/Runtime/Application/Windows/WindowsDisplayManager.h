#pragma once

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Application/DisplayManager.h"
#include "Application/Windows/WindowsScreen.h"
#include "Application/Windows/WindowsWindow.h"

namespace Kitsune
{
    class WindowsDisplayManager : public DisplayManager
    {
    public:
        WindowsDisplayManager(const WideStringView className);
        ~WindowsDisplayManager();

    public:
        void Update() override;

        ScreenHandle GetPrimaryScreen() const override;
        Array<ScreenHandle> GetScreens() const override;

    public:
        WindowHandle MakeWindow(const WindowSpecifications& specs) override;
        void DestroyWindow(WindowHandle window) override;

        WindowHandle GetPrimaryWindow() const override;

    private:
        void UpdateScreenList();

    private:
        static LRESULT WindowProc(HWND windowHandle, UINT message, WPARAM wparam,
                                  LPARAM lparam);

        static LRESULT HandlePreInitWindowEvents(HWND windowHandle, UINT message, WPARAM wparam,
                                                 LPARAM lparam);

        static LRESULT HandlePostInitWindowEvents(WindowsWindow* window, UINT message,
                                                  WPARAM wparam, LPARAM lparam);

    private:
        Array<ScopedPtr<WindowsScreen>> m_Screens;
        Array<ScopedPtr<WindowsWindow>> m_Windows;

        WideString m_WindowClassName;
    };
}
