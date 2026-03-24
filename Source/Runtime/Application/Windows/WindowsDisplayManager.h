#pragma once

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Application/DisplayManager.h"
#include "Application/Windows/WindowsScreen.h"
#include "Application/Windows/WindowsWindow.h"

#include "RenderingCore/RenderingDevice.h"
#include "RenderingCore/RenderingContext.h"

namespace Kitsune
{
    class WindowsDisplayManager : public DisplayManager
    {
    public:
        WindowsDisplayManager(const DisplayManagerSpecifications& specs,
                              const WideStringView className);

        ~WindowsDisplayManager();

    public:
        void Update() override;

        ScreenHandle GetPrimaryScreen() const override;
        Array<ScreenHandle> GetScreens() const override;

    public:
        WindowHandle GetPrimaryWindow() const override;

    private:
        void UpdateScreenList_();

    private:
        static LRESULT WindowProc_(HWND windowHandle, UINT message, WPARAM wparam,
                                   LPARAM lparam);

        static LRESULT HandlePreInitWindowEvents_(HWND windowHandle, UINT message, WPARAM wparam,
                                                  LPARAM lparam);

        static LRESULT HandlePostInitWindowEvents_(WindowsWindow* window, UINT message,
                                                   WPARAM wparam, LPARAM lparam);

    private:
        Array<ScopedPtr<WindowsScreen>> m_Screens;
        WindowsWindow* m_PrimaryWindow;

        WideString m_WindowClassName;

        RenderingContext* m_RenderingContext = nullptr;
        RenderingDevice* m_RenderingDevice = nullptr;
    };
}
