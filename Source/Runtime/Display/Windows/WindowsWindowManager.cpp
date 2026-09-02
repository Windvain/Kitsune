#include "Display/Windows/WindowsWindowManager.h"
#include <ShellScalingApi.h>

#include "Foundation/Logging/Logger.h"
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    WindowsWindowManager::WindowsWindowManager()
    {
        WNDCLASSEXW windowClass;
        HANDLE cursorImage = ::LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0,
                                         LR_DEFAULTSIZE | LR_SHARED);

        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;

        windowClass.lpfnWndProc = WindowProcedure;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;

        windowClass.hInstance = nullptr;
        windowClass.hIcon = nullptr;
        windowClass.hIconSm = nullptr;
        windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        windowClass.hCursor = reinterpret_cast<HCURSOR>(cursorImage);

        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = s_WindowClassName;

        if (!::RegisterClassExW(&windowClass))
            throw SystemException("Failed to register the window class.");
    }

    WindowsWindowManager::~WindowsWindowManager()
    {
        KITSUNE_VERIFY(
            ::UnregisterClassW(s_WindowClassName, nullptr),
            "Failed to unregister the window class.");
    }

    void WindowsWindowManager::Update(double delta)
    {
        KITSUNE_UNUSED(delta);

        MSG message;
        while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }

    WindowId WindowsWindowManager::CreateWindow(const WindowConfigurations& configs)
    {
        DWORD styles = GetWindowStyles(configs.Flags) | WS_VISIBLE;
        DWORD exStyles = GetWindowExStyles();

        RECT adjustedRect = {
            configs.Position.X,
            configs.Position.Y,
            LONG(configs.Position.X + configs.Size.X),
            LONG(configs.Position.Y + configs.Size.Y)
        };

        HMONITOR monitor = ::MonitorFromRect(&adjustedRect, MONITOR_DEFAULTTONEAREST);
        UINT dpiX, dpiY_;

        if (FAILED(::GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY_)))
        {
            KITSUNE_ENGINE_ERROR(
                Display,
                "Failed to retrieve DPI for the monitor closest "
                "to the soon-to-be created window. Using 96 DPI as a fallback.");

            dpiX = 96;
        }

        AdjustWindowRect(&adjustedRect, styles, false, exStyles, dpiX);

        ScopedPtr<WindowsWindow> window = MakeScoped<WindowsWindow>();
        WideString title = UTF8ToUTF16<char, wchar_t>(configs.Title);

        window->Fullscreen = false;
        window->Handle = ::CreateWindowExW(
            exStyles,
            s_WindowClassName,
            title.Raw(),
            styles,
            adjustedRect.left,
            adjustedRect.top,
            adjustedRect.right - adjustedRect.left,
            adjustedRect.bottom - adjustedRect.top,
            nullptr, nullptr, nullptr, nullptr);

        if (window->Handle == nullptr)
            throw SystemException("Failed to create a window using CreateWindowExW().");

        WindowsWindow* windowPointer = window.Get();
        auto windowId = reinterpret_cast<WindowId>(windowPointer);

        m_Windows.PushBack(Move(window));
        SetWindowState(windowId, configs.State);

        KITSUNE_UNUSED(::SetWindowLongPtrW(
            windowPointer->Handle,
            GWLP_USERDATA,
            reinterpret_cast<LPARAM>(windowPointer)));

        return windowId;
    }

    void WindowsWindowManager::DestroyWindow(WindowId windowId)
    {
        auto* window = reinterpret_cast<WindowsWindow*>(windowId);
        auto iter = Algorithms::Find(
            m_Windows.GetBegin(), m_Windows.GetEnd(), window);

        if (iter == m_Windows.GetEnd())
            throw InvalidArgumentException("The specified window is invalid.");

        KITSUNE_VERIFY(::DestroyWindow(window->Handle), "Failed to destroy the window.");
        m_Windows.RemoveUnsorted(iter);
    }

    Vector2<Uint32> WindowsWindowManager::GetWindowSize(WindowId windowId) const
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        auto handle = reinterpret_cast<WindowsWindow*>(windowId)->Handle;
        RECT rect = { 0, 0, 0, 0 };

        if (!::GetClientRect(handle, &rect))
            throw SystemException("Failed to get the window's size.");

        return { Uint32(rect.right), Uint32(rect.bottom) };
    }

    Vector2<Int32> WindowsWindowManager::GetWindowPosition(WindowId windowId) const
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        POINT position = { 0, 0 };
        auto handle = reinterpret_cast<WindowsWindow*>(windowId)->Handle;

        if (!::ClientToScreen(handle, &position))
            throw SystemException("Failed to get the window's position.");

        return { position.x, position.y };
    }

    String WindowsWindowManager::GetWindowTitle(WindowId windowId) const
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        auto handle = reinterpret_cast<WindowsWindow*>(windowId)->Handle;
        int length = ::GetWindowTextLengthW(handle);

        if (length == 0)
            return "";

        WideString wideTitle(length, '\0');
        int acquiredLength = ::GetWindowTextW(
            handle, wideTitle.Data(), static_cast<int>(wideTitle.Size() + 1));

        if (acquiredLength == 0)
            throw SystemException("Failed to get the window's title.");

        return UTF16ToUTF8<wchar_t, char>(wideTitle);
    }

    WindowState WindowsWindowManager::GetWindowState(WindowId windowId) const
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        auto* window = reinterpret_cast<WindowsWindow*>(windowId);
        if (::IsZoomed(window->Handle))
            return WindowState::Maximized;
        else if (::IsIconic(window->Handle))
            return WindowState::Minimized;

        // IMPORTANT: The fullscreen check should be AFTER the minimized check. This
        // is because a window can be both in fullscreen and minimized.
        if (window->Fullscreen)
            return WindowState::Fullscreen;
        else
            return WindowState::Windowed;
    }

    bool WindowsWindowManager::IsWindowVisible(WindowId windowId) const
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        return ::IsWindowVisible(reinterpret_cast<WindowsWindow*>(windowId)->Handle);
    }

    void WindowsWindowManager::SetWindowSize(
        WindowId windowId, const Vector2<Uint32>& size)
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        if (GetWindowState(windowId) != WindowState::Windowed)
        {
            KITSUNE_WARN_FORMAT(
                "Tried to set the size of a window (ID: {0}) when the window is "
                "not in the WindowState::Windowed state.",
                windowId);

            return;
        }

        auto handle = reinterpret_cast<WindowsWindow*>(windowId)->Handle;

        DWORD style = ::GetWindowLongPtrW(handle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(handle, GWL_EXSTYLE);

        RECT rect = { 0, 0, static_cast<LONG>(size.X), static_cast<LONG>(size.Y) };
        AdjustWindowRect(&rect, style, false, exStyle, GetWindowDPI(handle));

        BOOL success = ::SetWindowPos(
            handle, nullptr,
            0,
            0,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOMOVE);

        if (!success)
            throw SystemException("Failed to set the window's size.");
    }

    void WindowsWindowManager::SetWindowPosition(
        WindowId windowId, const Vector2<Int32>& position)
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        if (GetWindowState(windowId) != WindowState::Windowed)
        {
            KITSUNE_WARN_FORMAT(
                "Tried to set the position of a window (ID: {0}) when the window is "
                "not in the WindowState::Windowed state.",
                windowId);

            return;
        }

        auto handle = reinterpret_cast<WindowsWindow*>(windowId)->Handle;

        DWORD style = ::GetWindowLongPtrW(handle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(handle, GWL_EXSTYLE);

        auto size = static_cast<Vector2<LONG>>(GetWindowSize(windowId));
        RECT rect = { position.X, position.Y, size.X, size.Y };

        AdjustWindowRect(&rect, style, false, exStyle, GetWindowDPI(handle));
        if (!::SetWindowPos(handle, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE))
            throw SystemException("Failed to set the window's position.");
    }

    void WindowsWindowManager::SetWindowTitle(WindowId windowId, StringView title)
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        WideString wideTitle = UTF8ToUTF16<char, wchar_t>(title);
        auto handle = reinterpret_cast<WindowsWindow*>(windowId)->Handle;

        if (!::SetWindowTextW(handle, wideTitle.Raw()))
            throw SystemException("Failed to set the window's title.");
    }

    void WindowsWindowManager::SetWindowState(WindowId windowId, WindowState state)
    {
        auto* window = reinterpret_cast<WindowsWindow*>(windowId);
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        if (!IsWindowVisible(windowId))
        {
            KITSUNE_WARN_FORMAT(
                "Tried to set the state of an invisible window. (ID: {0}) This "
                "behaviour is platform-specific and will therefore be ignored. "
                "Call SetWindowVisibility() first.",
                windowId);

            return;
        }

        if (state == GetWindowState(windowId))
            return;

        // Revert fullscreen mode before calling ::ShowWindow.
        if (GetWindowState(windowId) == WindowState::Fullscreen)
        {
            ::SetWindowLongPtrW(window->Handle, GWL_STYLE, window->PrevStyle);
            if (!::SetWindowPlacement(window->Handle, &window->PrevPlacement))
            {
                throw SystemException(
                    "Failed to get the placement of the specified window.");
            }

            BOOL success = ::SetWindowPos(
                window->Handle,
                nullptr,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            if (!success)
                throw SystemException("Failed to set the specified window's position.");

            window->Fullscreen = false;
        }

        switch (state)
        {
        case WindowState::Windowed:
            ::ShowWindow(window->Handle, SW_RESTORE);
            break;
        case WindowState::Minimized:
            ::ShowWindow(window->Handle, SW_MINIMIZE);
            break;
        case WindowState::Maximized:
            ::ShowWindow(window->Handle, SW_MAXIMIZE);
            break;
        case WindowState::Fullscreen:
        {
            // Set the window back to windowed mode.
            if (GetWindowState(windowId) != WindowState::Windowed)
                ::ShowWindow(window->Handle, SW_RESTORE);

            // Thank you Raymond!
            // https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
            HMONITOR monitor = ::MonitorFromWindow(
                window->Handle,
                MONITOR_DEFAULTTONEAREST);

            MONITORINFO monitorInfo;
            monitorInfo.cbSize = sizeof(MONITORINFO);

            if (!::GetMonitorInfoW(monitor, &monitorInfo))
            {
                throw SystemException(
                    "Failed to get information about the closest monitor to the "
                    "window.");
            }

            if (!::GetWindowPlacement(window->Handle, &window->PrevPlacement))
            {
                throw SystemException(
                    "Failed to get the placement of the specified window.");
            }

            DWORD style = ::GetWindowLongPtrW(window->Handle, GWL_STYLE);
            window->PrevStyle = style;

            ::SetWindowLongPtrW(window->Handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            BOOL success = ::SetWindowPos(
                window->Handle,
                HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            if (!success)
                throw SystemException("Failed to set the specified window's position.");

            window->Fullscreen = true;
            break;
        }
        }
    }

    void WindowsWindowManager::SetWindowVisibility(WindowId windowId, bool visible)
    {
        if (IsWindowClosed(windowId))
        {
            throw InvalidArgumentException(
                "The specified window is invalid. It has been closed.");
        }

        DWORD showFlags = visible ? SW_SHOW : SW_HIDE;
        KITSUNE_UNUSED(::ShowWindow(
            reinterpret_cast<WindowsWindow*>(windowId)->Handle,
            showFlags));
    }

    DWORD WindowsWindowManager::GetWindowStyles(WindowCreationFlags flags)
    {
        DWORD styles = WS_OVERLAPPEDWINDOW;
        if (!bool(flags & WindowCreationFlags::ResizeEnabled))
            styles &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

        return styles;
    }

    DWORD WindowsWindowManager::GetWindowExStyles()
    {
        return WS_EX_APPWINDOW;
    }

    LRESULT WindowsWindowManager::WindowProcedure(
        HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
    {
        auto windowId = reinterpret_cast<WindowId>(
            ::GetWindowLongPtrW(handle, GWLP_USERDATA));

        // The window hasn't been fully created yet, send the messages over to
        // DefWindowProc.
        if (windowId == nullptr)
            return DefWindowProcW(handle, message, wparam, lparam);

        switch (message)
        {
        case WM_CLOSE:
            WindowManager::GetInstance()->DestroyWindow(windowId);
            break;

        default:
            return DefWindowProcW(handle, message, wparam, lparam);
        }

        return DefWindowProcW(handle, message, wparam, lparam);
    }
}
