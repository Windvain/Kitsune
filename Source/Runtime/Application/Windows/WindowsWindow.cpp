#include "Application/Windows/WindowsWindow.h"

#include "Foundation/String/UnicodeConversion.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    WindowsWindow::WindowsWindow(const wchar_t* className,
                                 const Vector2<int>& size, const Vector2<int>& position,
                                 const wchar_t* title, WindowFlags flags)
        : m_WindowFlags(flags), m_Fullscreen(false)
    {
        DWORD windowStyles = GetWindowStyles();
        DWORD exWindowStyles = GetExtendedWindowStyles();

        RECT adjustedRect = { position.x, position.y, position.x + size.x, position.y + size.y };
        ::AdjustWindowRectEx(&adjustedRect, windowStyles, false, exWindowStyles);

        m_WindowHandle = ::CreateWindowExW(GetExtendedWindowStyles(),
                                           className, title,
                                           GetWindowStyles(),
                                           adjustedRect.left,
                                           adjustedRect.top,
                                           adjustedRect.right - adjustedRect.left,
                                           adjustedRect.bottom - adjustedRect.top,
                                           nullptr, nullptr, nullptr, nullptr);

        if (m_WindowHandle == nullptr)
            throw SystemException("Failed to create a window using CreateWindowExW().");

        ::SetWindowLongPtrW(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
        ::ShowWindow(m_WindowHandle, SW_SHOW);
    }

    WindowsWindow::~WindowsWindow()
    {
        ::DestroyWindow(m_WindowHandle);
    }

    Vector2<Uint32> WindowsWindow::GetSize() const
    {
        RECT rect;
        ::GetClientRect(m_WindowHandle, &rect);

        return Vector2<Uint32>(static_cast<Uint32>(rect.right),
                               static_cast<Uint32>(rect.bottom));
    }

    Vector2<Int32> WindowsWindow::GetPosition() const
    {
        POINT clientPos = { 0, 0 };
        ::ClientToScreen(m_WindowHandle, &clientPos);

        return Vector2<Int32>(static_cast<Int32>(clientPos.x),
                              static_cast<Int32>(clientPos.y));
    }

    Vector2<Uint32> WindowsWindow::GetSizeWithDecorations() const
    {
        RECT windowRect = { 0, 0, 0, 0 };
        ::GetWindowRect(m_WindowHandle, &windowRect);

        return Vector2<Uint32>(windowRect.right - windowRect.left,
                               windowRect.bottom - windowRect.top);
    }

    Vector2<Int32> WindowsWindow::GetPositionWithDecorations() const
    {
        RECT windowRect = { 0, 0, 0, 0 };
        ::GetWindowRect(m_WindowHandle, &windowRect);

        return Vector2<Uint32>(windowRect.left, windowRect.top);
    }

    String WindowsWindow::GetTitle() const
    {
        int length = GetWindowTextLengthW(m_WindowHandle);
        WideString wideTitle(length, '\0');

        ::GetWindowTextW(m_WindowHandle, wideTitle.Data(), static_cast<int>(wideTitle.Size() + 1));
        return Unicode::ConvertString<wchar_t, char>(wideTitle);
    }

    void WindowsWindow::SetSize(const Vector2<Uint32>& size)
    {
        if (!IsWindowed() || !IsResizable())
            return;

        DWORD style = ::GetWindowLongPtrW(m_WindowHandle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(m_WindowHandle, GWL_EXSTYLE);

        RECT rect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };
        ::AdjustWindowRectEx(&rect, style, false, exStyle);

        ::SetWindowPos(m_WindowHandle, nullptr, 0, 0,
                        rect.right - rect.left,
                        rect.bottom - rect.top,
                        SWP_NOMOVE);
    }

    void WindowsWindow::SetPosition(const Vector2<Int32>& position)
    {
        if (!IsWindowed())
            return;

        DWORD style = ::GetWindowLongPtrW(m_WindowHandle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(m_WindowHandle, GWL_EXSTYLE);

        auto size = static_cast<Vector2<LONG>>(GetSize());
        RECT rect = { position.x, position.y, size.x, size.y };

        ::AdjustWindowRectEx(&rect, style, false, exStyle);
        ::SetWindowPos(m_WindowHandle, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE);
    }

    void WindowsWindow::SetTitle(const StringView title)
    {
        WideString wideTitle = Unicode::ConvertString<char, wchar_t>(title);
        ::SetWindowTextW(m_WindowHandle, wideTitle.Raw());
    }

    void WindowsWindow::Fullscreen()
    {
        if (IsFullscreen() || !IsShown())
            return;

        // Source from Raymond Chen: https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
        HMONITOR monitor = ::MonitorFromWindow(m_WindowHandle, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;

        monitorInfo.cbSize = sizeof(MONITORINFO);
        ::GetMonitorInfoW(monitor, &monitorInfo);

        ::GetWindowPlacement(m_WindowHandle, &m_PrevPlacement);

        DWORD style = GetWindowLongPtrW(m_WindowHandle, GWL_STYLE);
        ::SetWindowLongPtrW(m_WindowHandle, GWL_STYLE, style & ~GetWindowStyles());

        ::SetWindowPos(m_WindowHandle, HWND_TOP,
                       monitorInfo.rcMonitor.left,
                       monitorInfo.rcMonitor.top,
                       monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                       monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                       SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        m_Fullscreen = true;
    }

    void WindowsWindow::Maximize()
    {
        // ::ShowWindow() functions cannot maximize/minimize a hidden window.
        if (!IsMaximized() && IsShown())
            ::ShowWindow(m_WindowHandle, SW_MAXIMIZE);
    }

    void WindowsWindow::Minimize()
    {
        // ::ShowWindow() functions cannot maximize/minimize a hidden window.
        if (!IsMinimized() && IsShown())
            ::ShowWindow(m_WindowHandle, SW_MINIMIZE);
    }

    void WindowsWindow::Restore()
    {
        if (IsWindowed() || !IsShown())
            return;

        if (!m_Fullscreen)
            ::ShowWindow(m_WindowHandle, SW_RESTORE);
        else
        {
            DWORD styles = ::GetWindowLongPtrW(m_WindowHandle, GWL_STYLE);
            ::SetWindowLongPtrW(m_WindowHandle, GWL_STYLE, styles | GetWindowStyles());

            ::SetWindowPlacement(m_WindowHandle, &m_PrevPlacement);
            ::SetWindowPos(m_WindowHandle, nullptr, 0, 0, 0, 0,
                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                           SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            m_Fullscreen = false;
        }
    }

    DWORD WindowsWindow::GetExtendedWindowStyles() const
    {
        return WS_EX_APPWINDOW;
    }

    DWORD WindowsWindow::GetWindowStyles() const
    {
        DWORD styles = WS_OVERLAPPEDWINDOW;
        if ((m_WindowFlags & WindowFlags::FixedSize) == WindowFlags::FixedSize)
            styles &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

        return styles;
    }
}
