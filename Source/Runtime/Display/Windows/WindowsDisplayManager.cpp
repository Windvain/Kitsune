#include "Display/Windows/WindowsDisplayManager.h"
#include "Foundation/Logging/Logger.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"
#include "Foundation/Diagnostics/SingletonException.h"

namespace Kitsune
{
    WindowsDisplayManager* WindowsDisplayManager::s_Instance = nullptr;

    WindowsDisplayManager::WindowsDisplayManager(WideStringView className)
        : m_WindowClassName(className)
    {
        if (s_Instance != nullptr)
            throw SingletonException();

        WNDCLASSEXW windowClass;
        HANDLE cursorImage = ::LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0,
                                         LR_DEFAULTSIZE | LR_SHARED);

        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;

        windowClass.lpfnWndProc = WindowsDisplayManager::WindowProcedure;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;

        windowClass.hInstance = nullptr;
        windowClass.hIcon = nullptr;
        windowClass.hIconSm = nullptr;
        windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        windowClass.hCursor = reinterpret_cast<HCURSOR>(cursorImage);

        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = m_WindowClassName.Raw();

        if (!::RegisterClassExW(&windowClass))
            throw SystemException("Failed to register the window class.");

        UpdateDisplays();
        s_Instance = this;
    }

    WindowsDisplayManager::~WindowsDisplayManager()
    {
        KITSUNE_VERIFY(
            ::UnregisterClassW(m_WindowClassName.Raw(), nullptr),
            "Failed to unregister the window class.");

        s_Instance = nullptr;
    }

    void WindowsDisplayManager::Update()
    {
        UpdateDisplays();

        MSG message;
        while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }

    void WindowsDisplayManager::UpdateDisplays()
    {
        // HACK:
        // Turns out, neither HMONITOR nor DeviceName/szDevice persist when a monitor is
        // connected or disconnected.
        // Every single handle (HMONITOR) gets invalidated if the display changes.
        // The device names get shuffled around if the display changes.
        // I have no idea how else to approach this problem (maybe with EDIDs?) so this
        // is a half-baked solution y'all.

        DWORD index = 0;

        DISPLAY_DEVICE displayDevice{ /* ... */ };
        displayDevice.cb = sizeof(displayDevice);

        while (::EnumDisplayDevicesW(nullptr, index++, &displayDevice, 0))
        {
            if (!(displayDevice.StateFlags & DISPLAY_DEVICE_ACTIVE))
                continue;

            auto iter = Algorithms::FindIf(
                m_Displays.GetBegin(), m_Displays.GetEnd(),
                [&](const SharedPtr<WindowsDisplay>& display) -> bool
                {
                    return (display->GetDeviceName() == displayDevice.DeviceName);
                });

            if (iter == m_Displays.GetEnd())
            {
                m_OldDisplays.PushBack(
                    MakeShared<WindowsDisplay>(displayDevice.DeviceName));

                KITSUNE_ENGINE_INFO_FORMAT(
                    Display,
                    "A display with the name \"{0}\" has been connected.",
                    m_OldDisplays.Back()->GetDeviceString());
            }
            else
            {
                if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                    m_OldDisplays.Insert(m_OldDisplays.GetBegin(), Move(*iter));
                else
                    m_OldDisplays.PushBack(Move(*iter));

                m_Displays.RemoveUnsorted(iter);
            }
        }

        for (const SharedPtr<WindowsDisplay>& disconnected : m_Displays)
        {
            KITSUNE_INFO_FORMAT(
                "A display with the name \"{0}\" has been disconnected.",
                disconnected->GetDeviceString());
        }

        m_Displays.Clear();
        Swap(m_Displays, m_OldDisplays);
    }

    LRESULT WindowsDisplayManager::WindowProcedure(
        HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
    {
        auto* window = reinterpret_cast<WindowsWindow*>(
            ::GetWindowLongPtrW(handle, GWLP_USERDATA));

        switch (message)
        {
        case WM_CLOSE:
            window->Close();
            return 0;

        case WM_SYSCOMMAND:
        {
            // HACK: Even if we remove WS_SIZEBOX, the window can still be restored from
            // a maximized state to a windowed state. Catch that this is happening and
            // block it.
            if (window->IsUserResizable())
                break;

            WPARAM command = (wparam & ~0xF);       // The low 4 bits should be ignored.
            if (((command == SC_MOVE) || (command == SC_RESTORE)) && ::IsZoomed(handle))
                return 0;

            break;
        }
        default:
            break;      // Go to DefWindowProc!
        }

        return DefWindowProcW(handle, message, wparam, lparam);
    }
}
