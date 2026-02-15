#pragma once

#include <Windows.h>

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Application/DisplayManager.h"

namespace Kitsune
{
    class WindowsDisplayManager : public DisplayManager
    {
    public:
        WindowsDisplayManager();
        ~WindowsDisplayManager();

    public:
        void Update() override;

        SharedPtr<Screen> GetPrimaryScreen() const override;
        Array<SharedPtr<Screen>> GetScreens() const override;

    public:
        SharedPtr<Window> MakeWindow(const WindowSpecifications& specs) override;

    private:
        // Wrap the nested for loops in our own enumeration function, saves time writing code..
        using EnumerateMonitorsProc = bool (*)(const DISPLAY_DEVICEW&, const DISPLAY_DEVICEW&, void* data);
        static void EnumerateMonitors(EnumerateMonitorsProc procedure, void* data);

    private:
        static bool PrimaryEnumMonitorsProc(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor,
                                            void* untypedData);

        static bool RetrieveEnumMonitorsProc(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor,
                                             void* untypedData);

    private:
        static LRESULT WindowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam);

    private:
        static constexpr const wchar_t* s_WindowClassName = L"KitsuneWindowClass";
    };
}
