#include "ApplicationCore/Windows/WindowsWindow.h"

#include "Foundation/Threading/Interlocked.h"
#include "Foundation/String/UnicodeConversion.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/WindowException.h"

#include "ApplicationCore/CoreApplication.h"

// Copied from Windows.h, just made it work with signed integers instead.
#define KITSUNE_SIGNED_LOWORD_(lparam) static_cast<INT16>(static_cast<LONG_PTR>(lparam) & 0xFFFF)
#define KITSUNE_SIGNED_HIWORD_(lparam) static_cast<INT16>((static_cast<LONG_PTR>(lparam) >> 16) & 0xFFFF)

#define KITSUNE_CHECKED_CALL_(fn, ...) { if (fn) { fn(__VA_ARGS__); } }

namespace Kitsune
{
    volatile Int32 WindowsWindow::s_WindowCount = 0;

    WindowsWindow::WindowsWindow(int width, int height, int x, int y,
                                 const wchar_t* title, WindowState state, WindowFlag flags)
        : m_WindowFlags(flags)
    {
        WNDCLASSEXW windowClass = GetWindowClass();
        if (Interlocked::Load(&s_WindowCount) == 0)
            ::RegisterClassExW(&windowClass);

        DWORD exStyle = GetExtendedWindowStyles();
        DWORD style = GetWindowStyles();

        RECT adjustedRect = { x, y, x + width, y + height };
        ::AdjustWindowRectEx(&adjustedRect, style, false, exStyle);

        m_NativeHandle = ::CreateWindowExW(
            exStyle, s_WindowClassName,
            title, style,
            adjustedRect.left, adjustedRect.top,
            adjustedRect.right - adjustedRect.left,
            adjustedRect.bottom - adjustedRect.top,
            nullptr, nullptr, nullptr, nullptr);

        if (m_NativeHandle == nullptr)
            throw WindowException("Failed to create a window");

        ::SetWindowLongPtrW(m_NativeHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        ::ShowWindow(m_NativeHandle, SW_SHOW);
        SetState(state);

        if ((m_WindowFlags & WindowFlag::Resizable) == WindowFlag::None)
            ::GetWindowRect(m_NativeHandle, &m_UnresizableRect);
    }

    WindowsWindow::~WindowsWindow()
    {
        ::DestroyWindow(m_NativeHandle);

        if (Interlocked::Load(&s_WindowCount) != 1)
            Interlocked::Decrement(&s_WindowCount);
        else
            ::UnregisterClassW(s_WindowClassName, nullptr);
    }

    Vector2<Int32> WindowsWindow::GetPosition() const
    {
        POINT clientPos = { 0, 0 };
        ::ClientToScreen(m_NativeHandle, &clientPos);

        return { static_cast<Int32>(clientPos.x),
                 static_cast<Int32>(clientPos.y) };
    }

    Vector2<Uint32> WindowsWindow::GetSize() const
    {
        RECT rect;
        ::GetClientRect(m_NativeHandle, &rect);

        return { static_cast<Uint32>(rect.right),
                 static_cast<Uint32>(rect.bottom) };
    }

    void WindowsWindow::SetSize(const Vector2<Uint32>& size)
    {
        if (!IsWindowed()) Restore();

        RECT rect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };
        ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());

        ::SetWindowPos(m_NativeHandle, nullptr, 0, 0,
                       rect.right - rect.left,
                       rect.bottom - rect.top, SWP_NOMOVE);
    }

    void WindowsWindow::SetPosition(const Vector2<Int32>& pos)
    {
        if (!IsWindowed()) Restore();

        auto size = static_cast<Vector2<LONG>>(GetSize());
        RECT rect = { pos.x, pos.y, size.x, size.y };

        ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());
        ::SetWindowPos(m_NativeHandle, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE);
    }

    void WindowsWindow::SetTitle(StringView title)
    {
        WideString wideTitle;
        Unicode::Convert(title.GetBegin(), title.GetEnd(), BackInsertIterator<WideString>(wideTitle));

        ::SetWindowTextW(m_NativeHandle, wideTitle.Raw());
        m_Title = title;
    }

    void WindowsWindow::SetState(WindowState state)
    {
        // There is currently no way of changing the state of a window without showing it.
        // Wasn't worth rolling out our own ShowWindow(), because debugging was a pain due to
        // Microsoft's talent in not documenting anything.

        int showCmd = (state == WindowState::Maximized) ? SW_MAXIMIZE :
                      (state == WindowState::Minimized) ? SW_MINIMIZE :
                                                          SW_RESTORE;

        if (state == WindowState::Fullscreen)
            Fullscreen();
        else
        {
            if (m_Fullscreen) UndoFullscreen();
            ::ShowWindow(m_NativeHandle, showCmd);
        }
    }

    WindowState WindowsWindow::GetState() const
    {
        if (m_Fullscreen)
            return WindowState::Fullscreen;

        LONG style = ::GetWindowLongW(m_NativeHandle, GWL_STYLE);
        return (style & WS_MAXIMIZE) ? WindowState::Maximized :
               (style & WS_MINIMIZE) ? WindowState::Minimized :
                                       WindowState::Windowed;
    }

    void WindowsWindow::Fullscreen()
    {
        // Minimized -> Fullscreen doesn't work. Why? Who knows! Undocumented.
        // Source from Raymond Chen: https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353

        ::ShowWindow(m_NativeHandle, SW_RESTORE);
        m_Fullscreen = true;

        HMONITOR monitor = ::MonitorFromWindow(m_NativeHandle, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitorInfo;

        monitorInfo.cbSize = sizeof(MONITORINFO);
        ::GetMonitorInfoW(monitor, &monitorInfo);

        ::GetWindowPlacement(m_NativeHandle, &m_FullscreenPrev);

        DWORD style = GetWindowLong(m_NativeHandle, GWL_STYLE);
        ::SetWindowLongW(m_NativeHandle, GWL_STYLE, style & ~GetWindowStyles());

        RECT& monitorRect = monitorInfo.rcMonitor;
        ::SetWindowPos(m_NativeHandle, HWND_TOP, monitorRect.left, monitorRect.top,
                       monitorRect.right - monitorRect.left,
                       monitorRect.bottom - monitorRect.top,
                       SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    };

    void WindowsWindow::Show() { ::ShowWindow(m_NativeHandle, SW_SHOW); }
    void WindowsWindow::Hide() { ::ShowWindow(m_NativeHandle, SW_HIDE); }

    bool WindowsWindow::IsShown() const
    {
        return ::IsWindowVisible(m_NativeHandle);
    };

    WNDCLASSEXW WindowsWindow::GetWindowClass()
    {
        WNDCLASSEXW windowClass;
        windowClass.cbSize = sizeof(WNDCLASSEXW);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = KitsuneWindowProc;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;

        windowClass.hInstance = nullptr;
        windowClass.hIcon = nullptr;
        windowClass.hIconSm = nullptr;
        windowClass.hCursor = nullptr;
        windowClass.hbrBackground = nullptr;

        windowClass.lpszMenuName = L"";
        windowClass.lpszClassName = s_WindowClassName;

        return windowClass;
    }

    LRESULT WindowsWindow::KitsuneWindowProc(HWND windowHandle, UINT message,
                                             WPARAM wparam, LPARAM lparam)
    {
        auto& app = CoreApplication::GetInstance();
        auto* window = reinterpret_cast<WindowsWindow*>(::GetWindowLongPtrW(windowHandle, GWLP_USERDATA));

        if (window == nullptr)      // ::CreateWindowExW() calls the window procedure with WM_CREATE.
            return ::DefWindowProcW(windowHandle, message, wparam, lparam);

        switch (message)
        {
        case WM_CLOSE:
        {
            app.Exit(app.GetExitCode());
            return 0;
        }

        case WM_SIZING:
        {
            // Disabling WS_MAXIMIZEBOX and WS_SIZEBOX still allows for the window to be resized by
            // dragging the title bar.
            if ((window->m_WindowFlags & WindowFlag::Resizable) == WindowFlag::None)
            {
                RECT* rect = reinterpret_cast<RECT*>(lparam);
                *rect = window->m_UnresizableRect;

                return 0;
            }

            break;
        }

        case WM_SIZE:
        {
            Vector2<Uint32> size = { static_cast<Uint32>(LOWORD(lparam)),
                                     static_cast<Uint32>(HIWORD(lparam)) };

            if (wparam == SIZE_MINIMIZED)
            {
                KITSUNE_CHECKED_CALL_(window->m_MaximizeCallback);
            }
            else if (wparam == SIZE_MINIMIZED)
            {
                KITSUNE_CHECKED_CALL_(window->m_MinimizeCallback);
            }

            KITSUNE_CHECKED_CALL_(window->m_ResizeCallback, size);
            return 0;
        }

        case WM_MOVE:
        {
            // LOWORD() and HIWORD() underflow when dealing with negative position values.
            Vector2<Int32> position = { static_cast<Int32>(KITSUNE_SIGNED_LOWORD_(lparam)),
                                        static_cast<Int32>(KITSUNE_SIGNED_HIWORD_(lparam)) };

            KITSUNE_CHECKED_CALL_(window->m_MoveCallback, position);
            break;
        }
        }

        return ::DefWindowProcW(windowHandle, message, wparam, lparam);
    }

    DWORD WindowsWindow::GetExtendedWindowStyles() const
    {
        return WS_EX_APPWINDOW;
    }

    DWORD WindowsWindow::GetWindowStyles() const
    {
        DWORD styles = WS_OVERLAPPEDWINDOW;
        if ((m_WindowFlags & WindowFlag::Resizable) == WindowFlag::None)
            styles &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

        return styles;
    }

    void WindowsWindow::UndoFullscreen()
    {
        m_Fullscreen = false;

        LONG style = ::GetWindowLongW(m_NativeHandle, GWL_STYLE);
        SetWindowLong(m_NativeHandle, GWL_STYLE, style | GetWindowStyles());

        SetWindowPlacement(m_NativeHandle, &m_FullscreenPrev);
        SetWindowPos(m_NativeHandle, NULL, 0, 0, 0, 0,
                       SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                       SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}
