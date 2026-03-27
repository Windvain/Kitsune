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
        WindowsDisplayManager(
            WideStringView className,
            const DisplayManagerSpecifications& specs);

        ~WindowsDisplayManager() override;

    public:
        void Update() override;

        [[nodiscard]] ScreenHandle GetPrimaryScreen() const override;
        [[nodiscard]] Array<ScreenHandle> GetScreens() const override;

    public:
        [[nodiscard]] WindowHandle GetPrimaryWindow() const override;

    private:
        void RegisterWindowClass_(WideStringView className);
        void UpdateScreenList_();

        static void OnScreenConnected_(Screen* screen);
        static void OnScreenDisconnected_(Screen* screen);

        void InitializeRenderingContext_(RenderingBackend backend);

    private:
        static LRESULT WindowProcedure_(
            HWND windowHandle, UINT message, WPARAM wparam,
            LPARAM lparam);

        static LRESULT HandlePreInitWindowEvents_(
            HWND windowHandle, UINT message, WPARAM wparam,
            LPARAM lparam);

        static LRESULT HandlePostInitWindowEvents_(
            WindowsWindow* window, UINT message,
            WPARAM wparam, LPARAM lparam);

    private:
        Array<ScopedPtr<WindowsScreen>> m_Screens;
        WindowsWindow* m_PrimaryWindow;

        WideString m_WindowClassName;

        RenderingContext* m_RenderingContext = nullptr;
        RenderingDevice* m_RenderingDevice = nullptr;
    };
}
