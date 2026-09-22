#include "Display/Windows/WindowsWindow.h"
#include <ShellScalingApi.h>

#include "Display/Windows/WindowsDisplayManager.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/String/TranscodePresets.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    // Calling Open() will initialize the m_Handle variable.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    WindowsWindow::WindowsWindow(const WindowConfigurations& configurations)
    {
        Open(configurations);
    }

    WindowsWindow::~WindowsWindow()
    {
        if (IsOpen())
            Close();
    }

    void WindowsWindow::Open(const WindowConfigurations& configurations)
    {
        if (IsOpen())
        {
            throw SystemException(
                "Failed to open the window. This window object is already opened.");
        }

        auto* displayManager = WindowsDisplayManager::GetInstance();

        DWORD styles = GetWindowStyles(configurations.Flags);
        DWORD extendedStyles = GetWindowExtendedStyles();

        RECT windowRect = {
            configurations.Position.X,
            configurations.Position.Y,
            LONG(configurations.Position.X + configurations.Size.X),
            LONG(configurations.Position.Y + configurations.Size.Y)
        };

        AdjustRectFromStyles(
            &windowRect,
            styles,
            extendedStyles,
            GetDPIForClosestMonitor(&windowRect));

        WideString title = UTF8ToUTF16<char, wchar_t>(configurations.Title);
        m_Handle = ::CreateWindowExW(
            extendedStyles,
            displayManager->GetWindowClassName(),
            title.Raw(),
            styles,
            windowRect.left,
            windowRect.top,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr, nullptr, nullptr, nullptr);

        if (m_Handle == nullptr)
            throw SystemException("Failed to create a window.");

        if (!bool(configurations.Flags & WindowCreationFlags::Hidden))
            SetState(configurations.State);

        // If the previous value of the long pointer was 0 and the function succeeds, it
        // will still return 0, but not set the last error code.
        ::SetLastError(0);
        LONG_PTR result = ::SetWindowLongPtrW(
            m_Handle,
            GWLP_USERDATA,
            reinterpret_cast<LPARAM>(this));

        if ((result == 0) && (::GetLastError() != 0))
            throw SystemException("Failed to set the window's user-defined data.");
    }

    void WindowsWindow::Close()
    {
        // Don't throw in the destruction stage, just log and fail quietly.
        if (!IsOpen())
        {
            KITSUNE_ENGINE_ERROR(
                Display,
                "Failed to close the window. Tried to close a window which has "
                "not been opened.");

            return;
        }

        if (!::DestroyWindow(m_Handle))
        {
            DWORD error = ::GetLastError();
            KITSUNE_ENGINE_ERROR_FORMAT(
                Display,
                "Failed to destroy the window. ::DestroyWindow() returned an error "
                "code {0}", error);
        }

        // Reset fullscreen flag to make sure the next creation of a window with the
        // same object will not bug out.
        m_Fullscreen = false;
        m_Handle = nullptr;
    }

    bool WindowsWindow::IsOpen() const
    {
        return (m_Handle != nullptr);
    }

    Vector2<Uint32> WindowsWindow::GetSize() const
    {
        VerifyWindowIsOpen();

        RECT rect{ /* ... */ };
        if (!::GetClientRect(m_Handle, &rect))
            throw SystemException("Failed to get the window's size.");

        return { Uint32(rect.right), Uint32(rect.bottom) };
    }

    Vector2<Int32> WindowsWindow::GetPosition() const
    {
        VerifyWindowIsOpen();

        POINT position = { 0, 0 };
        if (!::ClientToScreen(m_Handle, &position))
            throw SystemException("Failed to get the window's position.");

        return { position.x, position.y };
    }

    String WindowsWindow::GetTitle() const
    {
        VerifyWindowIsOpen();

        int length = ::GetWindowTextLengthW(m_Handle);
        if (length == 0)
            return "";

        WideString wideTitle(length, '\0');
        int acquiredLength = ::GetWindowTextW(
            m_Handle, wideTitle.Data(), static_cast<int>(wideTitle.Size() + 1));

        if (acquiredLength == 0)
            throw SystemException("Failed to get the window's title.");

        return UTF16ToUTF8<wchar_t, char>(wideTitle);
    }

    WindowState WindowsWindow::GetState() const
    {
        VerifyWindowIsOpen();

        if (::IsZoomed(m_Handle))
            return WindowState::Maximized;
        else if (::IsIconic(m_Handle))
            return WindowState::Minimized;

        // IMPORTANT: The fullscreen check should be AFTER the minimized check. This
        // is because a window can be both in fullscreen and minimized.
        if (m_Fullscreen)
            return WindowState::Fullscreen;
        else
            return WindowState::Windowed;
    }

    bool WindowsWindow::IsVisible() const
    {
        VerifyWindowIsOpen();
        return ::IsWindowVisible(m_Handle);
    }

    void WindowsWindow::SetSize(const Vector2<Uint32>& size)
    {
        VerifyWindowIsOpen();

        if (GetState() != WindowState::Windowed)
        {
            KITSUNE_ENGINE_WARN(
                Display,
                "Tried to set the size of a window when the window is "
                "not in the WindowState::Windowed state.");

            return;
        }

        DWORD style = ::GetWindowLongPtrW(m_Handle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(m_Handle, GWL_EXSTYLE);

        RECT rect = { 0, 0, static_cast<LONG>(size.X), static_cast<LONG>(size.Y) };
        AdjustRectFromStyles(&rect, style, exStyle, ::GetDpiForWindow(m_Handle));

        BOOL success = ::SetWindowPos(
            m_Handle,
            nullptr,
            0,
            0,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOMOVE);

        if (!success)
            throw SystemException("Failed to set the window's size.");
    }

    void WindowsWindow::SetPosition(const Vector2<Int32>& position)
    {
        VerifyWindowIsOpen();

        if (GetState() != WindowState::Windowed)
        {
            KITSUNE_ENGINE_WARN(
                Display,
                "Tried to set the size of a window when the window is "
                "not in the WindowState::Windowed state.");

            return;
        }

        DWORD style = ::GetWindowLongPtrW(m_Handle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(m_Handle, GWL_EXSTYLE);

        auto size = static_cast<Vector2<LONG>>(GetSize());
        RECT rect = { position.X, position.Y, size.X, size.Y };

        AdjustRectFromStyles(&rect, style, exStyle, ::GetDpiForWindow(m_Handle));

        BOOL success = ::SetWindowPos(
            m_Handle,
            nullptr,
            rect.left, rect.top,
            0, 0,
            SWP_NOSIZE);

        if (!success)
            throw SystemException("Failed to set the window's position.");
    }

    void WindowsWindow::SetTitle(StringView title)
    {
        VerifyWindowIsOpen();

        WideString wideTitle = UTF8ToUTF16<char, wchar_t>(title);
        if (!::SetWindowTextW(m_Handle, wideTitle.Raw()))
            throw SystemException("Failed to set the window's title.");
    }

    void WindowsWindow::SetState(WindowState state)
    {
        VerifyWindowIsOpen();

        if (!IsVisible())
        {
            KITSUNE_ENGINE_WARN(
                Display,
                "Tried to set the state of an invisible window. This "
                "behaviour is platform-specific and will therefore be ignored. "
                "Call SetVisibility() first.");

            return;
        }

        if (state == GetState())
            return;

        // Revert fullscreen mode before calling ::ShowWindow.
        if (GetState() == WindowState::Fullscreen)
        {
            ::SetWindowLongPtrW(m_Handle, GWL_STYLE, m_PreviousStyle);
            if (!::SetWindowPlacement(m_Handle, &m_PreviousPlacement))
            {
                throw SystemException(
                    "Failed to get the placement of the specified window.");
            }

            BOOL success = ::SetWindowPos(
                m_Handle,
                nullptr,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            if (!success)
                throw SystemException("Failed to set the specified window's position.");

            m_Fullscreen = false;
        }

        switch (state)
        {
        case WindowState::Windowed:
            ::ShowWindow(m_Handle, SW_RESTORE);
            break;
        case WindowState::Minimized:
            ::ShowWindow(m_Handle, SW_MINIMIZE);
            break;
        case WindowState::Maximized:
            ::ShowWindow(m_Handle, SW_MAXIMIZE);
            break;
        case WindowState::Fullscreen:
        {
            // Set the window back to windowed mode.
            if (GetState() != WindowState::Windowed)
                ::ShowWindow(m_Handle, SW_RESTORE);

            // Thank you Raymond!
            // https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
            HMONITOR monitor = ::MonitorFromWindow(
                m_Handle,
                MONITOR_DEFAULTTONEAREST);

            MONITORINFO monitorInfo;
            monitorInfo.cbSize = sizeof(MONITORINFO);

            if (!::GetMonitorInfoW(monitor, &monitorInfo))
            {
                throw SystemException(
                    "Failed to get information about the closest monitor to the "
                    "window.");
            }

            if (!::GetWindowPlacement(m_Handle, &m_PreviousPlacement))
            {
                throw SystemException(
                    "Failed to get the placement of the specified window.");
            }

            DWORD style = ::GetWindowLongPtrW(m_Handle, GWL_STYLE);
            m_PreviousStyle = style;

            ::SetWindowLongPtrW(m_Handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            BOOL success = ::SetWindowPos(
                m_Handle,
                HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            if (!success)
                throw SystemException("Failed to set the specified window's position.");

            m_Fullscreen = true;
            break;
        }
        }
    }

    void WindowsWindow::SetVisibility(bool visible)
    {
        VerifyWindowIsOpen();

        int showFlags = visible ? SW_SHOW : SW_HIDE;
        ::ShowWindow(m_Handle, showFlags);
    }

    DWORD WindowsWindow::GetWindowStyles(WindowCreationFlags flags)
    {
        DWORD styles = WS_OVERLAPPEDWINDOW;
        if (!bool(flags & WindowCreationFlags::ResizeEnabled))
            styles &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

        if (!bool(flags & WindowCreationFlags::Hidden))
            styles |= WS_VISIBLE;

        return styles;
    }

    DWORD WindowsWindow::GetWindowExtendedStyles()
    {
        return WS_EX_APPWINDOW;
    }

    UINT WindowsWindow::GetDPIForClosestMonitor(RECT* rect)
    {
        HMONITOR monitor = ::MonitorFromRect(rect, MONITOR_DEFAULTTONEAREST);
        UINT dpiX, dpiY;

        if (FAILED(::GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
        {
            KITSUNE_ENGINE_ERROR(
                Display,
                "Failed to retrieve DPI for the monitor closest "
                "to the soon-to-be created window. Using 96 DPI as a fallback.");

            dpiX = 96;
        }

        return dpiX;
    }

    void WindowsWindow::AdjustRectFromStyles(
        RECT* rect, DWORD styles, DWORD extendedStyles, UINT dpi)
    {
        // If this fails, just... don't adjust the window rect.
        // It has minimal impact on how the application runs anyway.
        BOOL result = ::AdjustWindowRectExForDpi(
            rect, styles, false, extendedStyles, dpi);

        if (!result)
        {
            KITSUNE_ENGINE_ERROR(
                Display,
                "Failed to adjust the size of the window based on the given styles. "
                "Not adjusting the size of the window.");
        }
    }
}
