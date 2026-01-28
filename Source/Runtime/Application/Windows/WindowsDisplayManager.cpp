#include "Application/Windows/WindowsDisplayManager.h"

// #include "Launch/DefaultEngineLoop.h"
// #include "Foundation/String/UnicodeConversion.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"

#include "Application/Windows/WindowsScreen.h"
#include "Application/Windows/WindowsWindow.h"

namespace Kitsune
{
    WindowsDisplayManager::WindowsDisplayManager()
    {
        WNDCLASSEXW windowClass;
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;

        windowClass.lpfnWndProc = WindowProcedure;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;

        windowClass.hInstance = nullptr;
        windowClass.hIcon = nullptr;
        windowClass.hIconSm = nullptr;
        windowClass.hCursor = nullptr;
        windowClass.hbrBackground = nullptr;

        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = s_WindowClassName;

        if (!::RegisterClassExW(&windowClass))
            throw SystemException("Failed to register the window class.");
    }

    WindowsDisplayManager::~WindowsDisplayManager()
    {
        KITSUNE_VERIFY(::UnregisterClassW(s_WindowClassName, nullptr),
                       "Failed to unregister the window class.");
    }

    void WindowsDisplayManager::Update()
    {
        MSG message;
        while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }

    SharedPtr<Screen> WindowsDisplayManager::GetPrimaryScreen() const
    {
        SharedPtr<WindowsScreen> primaryScreen;
        EnumerateMonitors(PrimaryEnumMonitorsProc, &primaryScreen);

        return primaryScreen;
    }

    Array<SharedPtr<Screen>> WindowsDisplayManager::GetScreens() const
    {
        Array<SharedPtr<Screen>> screens;
        EnumerateMonitors(RetrieveEnumMonitorsProc, &screens);

        return screens;
    }

    SharedPtr<Window> WindowsDisplayManager::MakeWindow(const WindowSpecifications& specs)
    {
        WideString wideTitle = L""; //Unicode::ConvertString<char, wchar_t>(specs.Title);
        Vector2<Int32> position = specs.Position;

        if (specs.PositionHint == WindowPositionHint::PrimaryScreenCenter)
        {
            SharedPtr<Screen> primaryScreen = GetPrimaryScreen();
            position = primaryScreen->GetPosition() + ((primaryScreen->GetSize() - specs.Size) / 2);
        }

        return MakeShared<WindowsWindow>(s_WindowClassName, specs.Size, position, wideTitle.Raw(), specs.Flags);
    }

    void WindowsDisplayManager::EnumerateMonitors(EnumerateMonitorsProc procedure, void* data)
    {
        for (DWORD adapterIndex = 0; /* ... */; ++adapterIndex)
        {
            DISPLAY_DEVICEW adapterDevice;
            adapterDevice.cb = sizeof(adapterDevice);

            if (!::EnumDisplayDevicesW(nullptr, adapterIndex, &adapterDevice, 0))
                break;

            if (!(adapterDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
                continue;

            for (DWORD monitorIndex = 0; /* ... */; ++monitorIndex)
            {
                DISPLAY_DEVICEW monitorDevice;
                monitorDevice.cb = sizeof(monitorDevice);

                if (!::EnumDisplayDevicesW(adapterDevice.DeviceName, monitorIndex, &monitorDevice, 0))
                    break;

                if (!procedure(adapterDevice, monitorDevice, data))
                    break;
            }
        }
    }

    bool WindowsDisplayManager::PrimaryEnumMonitorsProc(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor,
                                                        void* untypedData)
    {
        auto* primaryScreen = static_cast<SharedPtr<WindowsScreen>*>(untypedData);
        if (monitor.StateFlags & (DISPLAY_DEVICE_PRIMARY_DEVICE | DISPLAY_DEVICE_ACTIVE))
        {
            *primaryScreen = MakeShared<WindowsScreen>(adapter, monitor);
            return false;
        }

        return true;
    }

    bool WindowsDisplayManager::RetrieveEnumMonitorsProc(const DISPLAY_DEVICEW& adapter, const DISPLAY_DEVICEW& monitor,
                                                         void* untypedData)
    {
        auto* screens = static_cast<Array<SharedPtr<Screen>>*>(untypedData);
        if (monitor.StateFlags & DISPLAY_DEVICE_ACTIVE)
            screens->PushBack(MakeShared<WindowsScreen>(adapter, monitor));

        return true;
    }

    LRESULT WindowsDisplayManager::WindowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam)
    {
        (void)windowHandle;
        (void)message;
        (void)wparam;
        (void)lparam;
        // All events in the window procedure are dispatched immediately.
        // This is due to the Win32 API blocking the main thread from running until the event
        // is finished.
        /*
        DefaultEngineLoop* engineLoop = DefaultEngineLoop::GetInstance();
        if (engineLoop->GetApplication() == nullptr)
            return ::DefWindowProcW(windowHandle, message, wparam, lparam);

        WindowsDisplayManager* dispManager = dynamic_cast<WindowsDisplayManager*>(DisplayManager::GetInstance());
        KITSUNE_ASSERT(dispManager != nullptr, "The display maanger instance is not a windows display manager?");

        switch (message)
        {
        case WM_CLOSE:
        {
            break;
        }
        case WM_DESTROY:
        {
            break;
        }
        case WM_SIZE:
        {
            Vector2<Uint32> newSize(LOWORD(lparam), HIWORD(lparam));
            engineLoop->GetApplication()->OnViewportResize(newSize);

            break;
        }
        default:
            return DefWindowProcW(windowHandle, message, wparam, lparam);
        }
*/
        return 0;
    }

    DisplayManager* DisplayManager::Create()
    {
        return Memory::New<WindowsDisplayManager>();
    }

    void DisplayManager::Destroy()
    {
        Memory::Delete(GetInstance());
    }
}
