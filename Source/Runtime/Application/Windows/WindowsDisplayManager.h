#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Application/IDisplayManager.h"
#include "Application/Windows/WindowsScreen.h"
#include "Application/Windows/WindowsWindow.h"

namespace Kitsune
{
    class WindowsDisplayManager : public IDisplayManager
    {
    public:
        WindowsDisplayManager();
        ~WindowsDisplayManager();

    public:
        void Update() override;

        SharedPtr<IScreen> GetPrimaryScreen() const override;
        Array<SharedPtr<IScreen>> GetScreens() const override;

    public:
        SharedPtr<IWindow> RegisterWindow(const WindowSpecifications& specs) override;
        inline SharedPtr<IWindow> GetPrimaryWindow() const override
        {
            // TODO: This code is temporary. Ideally you would mark the first created window with a boolean or something.
            return m_Windows[0];
        }

    public:
        inline Array<SharedPtr<WindowsWindow>>& GetPlatformWindows() { return m_Windows; }
        inline const Array<SharedPtr<WindowsWindow>>& GetPlatformWindows() const { return m_Windows; }

    private:
        using EnumerateMonitorsProc = bool (*)(const DISPLAY_DEVICEW&, const DISPLAY_DEVICEW&, void* data);
        static void EnumerateMonitors(EnumerateMonitorsProc procedure, void* data);

        struct PrimaryEnumMonitorsProcData  { SharedPtr<WindowsScreen> Screen; };
        struct RetrieveEnumMonitorsProcData { Array<SharedPtr<IScreen>> Screens; };

        static bool PrimaryEnumMonitorsProc(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor,
                                            void* untypedData);

        static bool RetrieveEnumMonitorsProc(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor,
                                             void* untypedData);

    private:
        static LRESULT WindowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam);

    private:
        static constexpr const wchar_t* s_WindowClassName = L"KitsuneWindowClass";

    private:
        Array<SharedPtr<WindowsWindow>> m_Windows;
    };
}
