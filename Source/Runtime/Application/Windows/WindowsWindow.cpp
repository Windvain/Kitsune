#include "Application/Windows/WindowsWindow.h"
#include <ShellScalingApi.h>

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    WindowsWindow::WindowsWindow(WideStringView className,
                                 const WindowSpecifications& specs)
        : m_Flags(specs.Flags)
    {
        DWORD styles = CalculateWindowStyles_(specs.Flags) | WS_VISIBLE;
        DWORD extendedStyles = CalculateWindowExStyles_(specs.Flags);

        // Get the DPI of the monitor closest to the window.
        RECT windowRect = {
            specs.Position.X,
            specs.Position.Y,
            static_cast<LONG>(specs.Position.X + specs.Size.X),
            static_cast<LONG>(specs.Position.Y + specs.Size.Y)
        };

        HMONITOR monitor = ::MonitorFromRect(
            &windowRect,
            MONITOR_DEFAULTTONEAREST);

        UINT dpiX, dpiY_;
        HRESULT getDpiResult = ::GetDpiForMonitor(
            monitor, MDT_EFFECTIVE_DPI,
            &dpiX, &dpiY_);

        if (FAILED(getDpiResult))
        {
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Failed to retrieve DPI for the monitor closest "
                "to the soon-to-be created window. Using 96 DPI as "
                "a fallback.");

            dpiX = 96;
        }

        AdjustWindowRectExForDpi_(
            &windowRect, styles, false, extendedStyles,
            dpiX);

        WideString wideTitle = Utf8ToUtf16<char, wchar_t>(specs.Title.Raw());
        m_Handle = ::CreateWindowExW(
            extendedStyles,
            className.Data(),
            wideTitle.Raw(),
            styles,
            windowRect.left,
            windowRect.top,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr, nullptr, nullptr, nullptr);

        if (m_Handle == nullptr)
            throw SystemException("Failed to create a window.");

        ::SetWindowLongPtrW(m_Handle, GWLP_USERDATA,
                            reinterpret_cast<LPARAM>(this));

        SetMode(specs.Mode);

        KITSUNE_ENGINE_INFO_FORMAT(
            DisplayManager,
            "Created a Win32 window with the specifications [ Size: "
            "({0}, {1}), Position: ({2}, {3}), Title: \"{4}\" ]",
            specs.Size.X, specs.Size.Y,
            specs.Position.X, specs.Position.Y,
            specs.Title);
    }

    WindowsWindow::~WindowsWindow()
    {
        KITSUNE_VERIFY(
            ::DestroyWindow(m_Handle),
            "Failed to destroy a window. Did you try to destroy "
            "a window which was created by a different thread?");
    }

    Vector2<Uint32> WindowsWindow::GetSize() const
    {
        RECT clientRect = { 0, 0, 0, 0 };
        if (!::GetClientRect(m_Handle, &clientRect))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to retrieve the client area of the window with handle {0}."
                "Returning (0, 0)",
                m_Handle);
        }

        return {
            static_cast<Uint32>(clientRect.right),
            static_cast<Uint32>(clientRect.bottom)
        };
    }

    Vector2<Int32> WindowsWindow::GetPosition() const
    {
        POINT clientPos = { 0, 0 };
        if (!::ClientToScreen(m_Handle, &clientPos))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to retrieve the position of the window with handle {0}."
                "Returning (0, 0).",
                m_Handle);
        }

        return {
            static_cast<Int32>(clientPos.x),
            static_cast<Int32>(clientPos.y)
        };
    }

    Vector2<Uint32> WindowsWindow::GetSizeWithDecorations() const
    {
        RECT windowRect = { 0, 0, 0, 0 };
        if (!::GetWindowRect(m_Handle, &windowRect))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to retrieve size of the window with handle {0}."
                "Returning a size of (0, 0).",
                m_Handle);
        }

        return {
            static_cast<Uint32>(windowRect.right - windowRect.left),
            static_cast<Uint32>(windowRect.bottom - windowRect.top)
        };
    }

    Vector2<Int32> WindowsWindow::GetPositionWithDecorations() const
    {
        RECT windowRect = { 0, 0, 0, 0 };
        if (!::GetWindowRect(m_Handle, &windowRect))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to retrieve position of the window with handle {0}. "
                "Returning (0, 0) as a fallback value.",
                m_Handle);
        }

        return { windowRect.left, windowRect.top };
    }

    String WindowsWindow::GetTitle() const
    {
        int titleSize = GetWindowTextLengthW(m_Handle);
        if (titleSize == 0)
            return "";

        WideString wideTitle(titleSize, '\0');
        int copiedLength = ::GetWindowTextW(
            m_Handle, wideTitle.Data(), titleSize + 1);

        if (copiedLength == 0)
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to retrieve title of the window with handle {0}.",
                m_Handle);

            return "";
        }

        return Utf16ToUtf8<wchar_t, char>(wideTitle);
    }

    WindowMode WindowsWindow::GetMode() const
    {
        if (::IsZoomed(m_Handle))
            return WindowMode::Maximized;
        else if (::IsIconic(m_Handle))
            return WindowMode::Minimized;
        else if (m_Fullscreen)
            return WindowMode::Fullscreen;
        else
            return WindowMode::Windowed;
    }

    void WindowsWindow::SetSize(const Vector2<Uint32>& size)
    {
        if (GetMode() != WindowMode::Windowed)
        {
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Cannot set the size of a non-restored window.");

            return;
        }

        RECT windowRect = {
            0, 0,
            static_cast<LONG>(size.X),
            static_cast<LONG>(size.Y)
        };

        AdjustWindowRectExForDpi_(
            &windowRect,
            ::GetWindowLongPtrW(m_Handle, GWL_STYLE),
            false,
            ::GetWindowLongPtrW(m_Handle, GWL_EXSTYLE),
            GetDpiForWindow_(m_Handle));

        BOOL success = ::SetWindowPos(
            m_Handle, nullptr, 0, 0,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

        if (!success)
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to set the size of the window with handle {0}.",
                m_Handle);
        }
    }

    void WindowsWindow::SetPosition(const Vector2<Int32>& position)
    {
        if (GetMode() != WindowMode::Windowed)
        {
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Cannot set the position of a non-restored window.");

            return;
        }

        RECT windowRect = { position.X, position.Y, position.X, position.Y };
        AdjustWindowRectExForDpi_(
            &windowRect,
            ::GetWindowLongPtrW(m_Handle, GWL_STYLE),
            false,
            ::GetWindowLongPtrW(m_Handle, GWL_EXSTYLE),
            GetDpiForWindow_(m_Handle));

        if (!::SetWindowPos(m_Handle, nullptr, windowRect.left, windowRect.top,
                            0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to set the position of the window with the handle {0}.",
                m_Handle);
        }
    }

    void WindowsWindow::SetTitle(StringView title)
    {
        WideString wideTitle = Utf8ToUtf16<char, wchar_t>(title);
        if (!::SetWindowTextW(m_Handle, wideTitle.Raw()))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to set title of the window with the handle {0}.",
                m_Handle);
        }
    }

    void WindowsWindow::SetMode(WindowMode mode)
    {
        if (mode == GetMode())
            return;

        if (!IsVisible())
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Tried to set the mode of an invisible window with "
                "the handle {0}. Make the window visible with "
                "SetVisibility() before calling this function!",
                m_Handle);
        }

        // Revert the changes made by making the window fullscreen, then set
        // the mode using ShowWindow().
        if (m_Fullscreen)
            DisableFullscreen_();

        switch (mode)
        {
        case WindowMode::Windowed:
        {
            KITSUNE_UNUSED(::ShowWindow(m_Handle, SW_RESTORE));
            break;
        }
        case WindowMode::Minimized:
        {
            KITSUNE_UNUSED(::ShowWindow(m_Handle, SW_SHOWMINIMIZED));
            break;
        }
        case WindowMode::Maximized:
        {
            KITSUNE_UNUSED(::ShowWindow(m_Handle, SW_MAXIMIZE));
            break;
        }
        case WindowMode::Fullscreen:
        {
            EnableFullscreen_();
            break;
        }
        }
    }

    void WindowsWindow::SetVisibility(bool visible)
    {
        DWORD showFlags = visible ? SW_SHOW : SW_HIDE;
        KITSUNE_UNUSED(::ShowWindow(m_Handle, showFlags));
    }

    DWORD WindowsWindow::CalculateWindowStyles_(WindowFlags flags)
    {
        DWORD styles = WS_OVERLAPPEDWINDOW;
        if ((flags & WindowFlags::ResizeDisabled) == WindowFlags::ResizeDisabled)
            styles &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

        return styles;
    }

    DWORD WindowsWindow::CalculateWindowExStyles_(WindowFlags flags)
    {
        KITSUNE_UNUSED(flags);
        return WS_EX_APPWINDOW;
    }

    void WindowsWindow::EnableFullscreen_()
    {
        // Thank you Raymond!
        // https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
        HMONITOR monitor = ::MonitorFromWindow(
            m_Handle,
            MONITOR_DEFAULTTONEAREST);

        MONITORINFO monitorInfo;
        monitorInfo.cbSize = sizeof(MONITORINFO);

        if (!::GetMonitorInfoW(monitor, &monitorInfo))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to get information about the closest monitor "
                "to the window. Could not set the window with handle {0} "
                "to fullscreen mode.",
                m_Handle);

            return;
        }

        if (!::GetWindowPlacement(m_Handle, &m_PreviousPlacement))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to get the placement of the window with handle {0}. Could "
                "not set the window to fullscreen mode.",
                m_Handle);

            return;
        }

        ::SetWindowLongPtrW(
            m_Handle, GWL_STYLE,
            ::GetWindowLongPtrW(m_Handle, GWL_STYLE) & ~WS_OVERLAPPEDWINDOW);

        BOOL success = ::SetWindowPos(
            m_Handle, HWND_TOP,
            monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.top,
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

        if (!success)
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to fullscreen the window with handle {0}.",
                m_Handle);
        }

        m_Fullscreen = true;
    }

    void WindowsWindow::DisableFullscreen_()
    {
        // Recalculate the window styles because we messed around with them
        // by setting the window to fullscreen.
        ::SetWindowLongPtrW(m_Handle, GWL_STYLE, CalculateWindowStyles_(m_Flags));
        ::SetWindowLongPtrW(m_Handle, GWL_EXSTYLE, CalculateWindowExStyles_(m_Flags));

        if (!::SetWindowPlacement(m_Handle, &m_PreviousPlacement))
        {
            KITSUNE_ENGINE_ERROR_FORMAT(
                DisplayManager,
                "Failed to restore the window with handle {0} "
                "back from fullscreen mode.",
                m_Handle);
        }

        KITSUNE_UNUSED(::SetWindowPos(
            m_Handle, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
            SWP_FRAMECHANGED));

        m_Fullscreen = false;
    }

    BOOL WindowsWindow::AdjustWindowRectExForDpi_(
        LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
        DWORD dwExStyle, UINT dpi)
    {
#if !defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
        return ::AdjustWindowRectExForDpi(
            lpRect, dwStyle, bMenu, dwExStyle, dpi);
#else
        // MinGW doesn't load the DPI-aware functions.
        using AdjustWindowRectExForDpiFunc =
            BOOL (*)(LPRECT, DWORD, BOOL, DWORD, UINT);

        HMODULE user32DllHandle = ::GetModuleHandleW(L"user32.dll");
        if (user32DllHandle == nullptr)
        {
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Failed to get a handle to User32.dll. This is usually "
                "due to the application not being default-linked to "
                "User32.dll.");
        }

        auto function = (AdjustWindowRectExForDpiFunc)(void*)(
            ::GetProcAddress(user32DllHandle, "AdjustWindowRectExForDpi"));

        if (function == nullptr)
        {
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Failed to retrieve AdjustWindowRectExForDpi() from "
                "User32.dll. This might be because your Windows installation "
                "is too old. Falling back to AdjustWindowRectEx.");

            return ::AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
        }

        return function(lpRect, dwStyle, bMenu, dwExStyle, dpi);
#endif
    }

    UINT WindowsWindow::GetDpiForWindow_(HWND hwnd)
    {
#if !defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
        return ::GetDpiForWindow(hwnd);
#else
        // MinGW doesn't load the DPI-aware functions.
        using GetDpiForWindowFunc = UINT (*)(HWND);

        HMODULE user32Dll = ::GetModuleHandleW(L"user32.dll");
        if (user32Dll == nullptr)
        {
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Failed to get a handle to User32.dll. This is usually "
                "due to the application not being default-linked to "
                "User32.dll.");
        }

        auto function = (GetDpiForWindowFunc)(void*)(
            ::GetProcAddress(user32Dll, "GetDpiForWindow"));

        if (function == nullptr)
        {
            KITSUNE_ENGINE_ERROR(
                DisplayManager,
                "Failed to retrieve GetDpiForWindow() from User32.dll. "
                "This might be because your Windows installation "
                "is too old. Falling back to 96 DPI.");

            return 96;
        }

        return function(hwnd);
#endif
    }
}
