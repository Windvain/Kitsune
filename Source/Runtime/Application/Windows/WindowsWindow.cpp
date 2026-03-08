#include "Application/Windows/WindowsWindow.h"
#include <ShellScalingApi.h>

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    WindowsWindow::WindowsWindow(const WideStringView className,
                                 const Vector2<int>& size,
                                 const Vector2<int>& position,
                                 const WideStringView title,
                                 WindowMode mode,
                                 WindowFlags flags)
        : m_Flags(flags)
    {
        DWORD styles = GetWindowStyles_(flags) | WS_VISIBLE;
        DWORD exStyles = GetWindowExStyles_();

        RECT adjustedRect = {
            position.X,
            position.Y,
            position.X + size.X,
            position.Y + size.Y
        };

        HMONITOR monitor = ::MonitorFromRect(&adjustedRect, MONITOR_DEFAULTTONEAREST);
        UINT dpiX, dpiY_;

        if (FAILED(::GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY_)))
        {
            KITSUNE_ENGINE_ERROR_("Failed to retrieve DPI for the monitor closest "
                                  "to the soon-to-be created window. Using 96 DPI as a fallback.");

            // As to why only the X value is used but not the Y value,
            // check WindowsScreen.cpp.
            dpiX = 96;
        }

        if (!AdjustWindowRectExForDpi_(&adjustedRect, styles, false, exStyles, dpiX))
        {
            KITSUNE_ENGINE_ERROR_("AdjustWindowRectExForDpi_() failed, are the styles and "
                                  "extra styles valid?");
        }

        m_Handle = ::CreateWindowExW(
            exStyles,
            className.Data(),
            title.Data(),
            styles,
            adjustedRect.left,
            adjustedRect.top,
            adjustedRect.right - adjustedRect.left,
            adjustedRect.bottom - adjustedRect.top,
            nullptr, nullptr, nullptr, nullptr);

        if (m_Handle == nullptr)
            throw SystemException("Failed to create a window using CreateWindowExW().");

        ::SetWindowLongPtrW(m_Handle, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
        SetMode(mode);
    }

    WindowsWindow::~WindowsWindow()
    {
        KITSUNE_VERIFY(::DestroyWindow(m_Handle),
                       "Failed to destroy a window. Did you try to destroy "
                       "a window which was created by a different thread?");
    }

    Vector2<Uint32> WindowsWindow::GetSize() const
    {
        RECT rect;
        if (!::GetClientRect(m_Handle, &rect))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to retrieve the client area of "
                                         "window {0}.", this);

            return Vector2<Uint32>();
        }

        return {
            static_cast<Uint32>(rect.right),
            static_cast<Uint32>(rect.bottom)
        };
    }

    Vector2<Int32> WindowsWindow::GetPosition() const
    {
        POINT clientPos = { 0, 0 };
        if (!::ClientToScreen(m_Handle, &clientPos))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to retrieve the position of window "
                                         "{0}.", this);

            return Vector2<Int32>();
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
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to retrieve the size of window {0}.",
                                         this);

            return Vector2<Uint32>();
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
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to retrieve the position of window {0}.",
                                         this);

            return Vector2<Uint32>();
        }

        return { windowRect.left, windowRect.top };
    }

    String WindowsWindow::GetTitle() const
    {
        int length = GetWindowTextLengthW(m_Handle);
        if (length == 0)
            return "";

        WideString wideTitle(length, '\0');
        int acquiredLength = ::GetWindowTextW(
            m_Handle, wideTitle.Data(),
            static_cast<int>(wideTitle.Size() + 1));

        if (acquiredLength == 0)
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to retrieve the title of window {0}", this);
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
            KITSUNE_ENGINE_ERROR_FORMAT_("Tried to set the size of a non-restored "
                                         "window {0}", this);

            return;
        }

        DWORD style = ::GetWindowLongPtrW(m_Handle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(m_Handle, GWL_EXSTYLE);

        RECT rect = { 0, 0, static_cast<LONG>(size.X), static_cast<LONG>(size.Y) };
        if (!AdjustWindowRectExForDpi_(&rect, style, false, exStyle, GetDpiForWindow_(m_Handle)))
        {
            KITSUNE_ENGINE_ERROR_("AdjustWindowRectExForDpi_() failed, this might be due to "
                                  "invalid arguments being passed in.");
        }

        BOOL success = ::SetWindowPos(
            m_Handle, nullptr,
            0,
            0,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_NOMOVE);

        if (!success)
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to set the size of window {0}.", this);
    }

    void WindowsWindow::SetPosition(const Vector2<Int32>& position)
    {
        if (GetMode() != WindowMode::Windowed)
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Tried to set the position of a non-restored "
                                         "window {0}", this);

            return;
        }

        DWORD style = ::GetWindowLongPtrW(m_Handle, GWL_STYLE);
        DWORD exStyle = ::GetWindowLongPtrW(m_Handle, GWL_EXSTYLE);

        auto size = static_cast<Vector2<LONG>>(GetSize());
        RECT rect = { position.X, position.Y, size.X, size.Y };

        if (!AdjustWindowRectExForDpi_(&rect, style, false, exStyle, GetDpiForWindow_(m_Handle)))
        {
            KITSUNE_ENGINE_ERROR_("AdjustWindowRectExForDpi_() failed, is this due to "
                                  "invalid arguments?");
        }

        if (!::SetWindowPos(m_Handle, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE))
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to set the position of window {0}.", this);
    }

    void WindowsWindow::SetTitle(const StringView title)
    {
        WideString wideTitle = Utf8ToUtf16<char, wchar_t>(title);
        if (!::SetWindowTextW(m_Handle, wideTitle.Raw()))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("Failed to set the title of window {0}.", this);
        }
    }

    void WindowsWindow::SetMode(WindowMode mode)
    {
        bool resizeDisabled = (bool)(m_Flags & WindowFlags::ResizeDisabled);
        if (resizeDisabled && (mode != WindowMode::Windowed) && (mode != WindowMode::Minimized))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_(
                "Tried to set the mode of a non-resizable window {0} to a mode other than "
                "WindowMode::Windowed or WindowMode::Minimized.",
                this);

            return;
        }

        if (!IsVisible())
        {
            KITSUNE_ENGINE_ERROR_FORMAT_(
                "Tried to set the mode of an invisible window {0}. "
                "Make the window visible with SetVisibility() before "
                "calling this function!",
                this);
        }

        // If the window is currently in fullscreen mode, restore it to normal before
        // using the ShowWindow() functions.
        if (m_Fullscreen)
        {
            ::SetWindowLongPtrW(m_Handle, GWL_STYLE, GetWindowStyles_(m_Flags));
            ::SetWindowLongPtrW(m_Handle, GWL_EXSTYLE, GetWindowExStyles_());

            if (!::SetWindowPlacement(m_Handle, &m_PreviousPlacement))
            {
                KITSUNE_ENGINE_ERROR_FORMAT_("Failed to set the placement of window "
                                             "{0}.", this);
            }

            BOOL success =  ::SetWindowPos(m_Handle, nullptr, 0, 0, 0, 0,
                                           SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                           SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            if (!success)
            {
                KITSUNE_ENGINE_ERROR_FORMAT_("Failed to update window {0} via SetWindowPos().",
                                             this);
            }

            m_Fullscreen = false;
        }

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
            // Thank you Raymond!
            // https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
            HMONITOR monitor = ::MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);
            MONITORINFO monitorInfo;

            monitorInfo.cbSize = sizeof(MONITORINFO);
            if (!::GetMonitorInfoW(monitor, &monitorInfo))
            {
                KITSUNE_ENGINE_ERROR_("Failed to get the closest monitor's info.");
                return;
            }

            if (!::GetWindowPlacement(m_Handle, &m_PreviousPlacement))
            {
                KITSUNE_ENGINE_ERROR_FORMAT_("Failed to get placement of window {0}.", this);
                return;
            }

            DWORD style = ::GetWindowLongPtrW(m_Handle, GWL_STYLE);
            ::SetWindowLongPtrW(m_Handle, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

            BOOL success = ::SetWindowPos(m_Handle, HWND_TOP,
                                          monitorInfo.rcMonitor.left,
                                          monitorInfo.rcMonitor.top,
                                          monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                                          monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                                          SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

            if (!success)
            {
                KITSUNE_ENGINE_ERROR_FORMAT_(
                    "Failed to set the position and size of window {0}.",
                    this);
            }

            m_Fullscreen = true;
            break;
        }
        }
    }

    void WindowsWindow::SetVisibility(bool visible)
    {
        DWORD showFlags = visible ? SW_SHOW : SW_HIDE;
        KITSUNE_UNUSED(::ShowWindow(m_Handle, showFlags));
    }

    DWORD WindowsWindow::GetWindowStyles_(WindowFlags flags)
    {
        DWORD styles = WS_OVERLAPPEDWINDOW;
        if ((flags & WindowFlags::ResizeDisabled) == WindowFlags::ResizeDisabled)
            styles &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);

        return styles;
    }

    DWORD WindowsWindow::GetWindowExStyles_()
    {
        return WS_EX_APPWINDOW;
    }

    BOOL WindowsWindow::AdjustWindowRectExForDpi_(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
                                                  DWORD dwExStyle, UINT dpi)
    {
#if defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
        // MinGW doesn't load the DPI-aware functions.
        using FunctionSignature = BOOL (*)(LPRECT, DWORD, BOOL, DWORD, UINT);

        HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
        if (user32 == nullptr)
            KITSUNE_ENGINE_ERROR_("Failed to get a handle to User32.dll.");

        auto function = (FunctionSignature)(void*)(::GetProcAddress(
            user32, "AdjustWindowRectExForDpi"));

        if (function == nullptr)
        {
            KITSUNE_ENGINE_ERROR_("Failed to retrieve AdjustWindowRectExForDpi() from "
                                  "User32.dll. This might be because your Windows installation "
                                  "is too old. Falling back to AdjustWindowRectEx.");

            return ::AdjustWindowRectEx(lpRect, dwStyle, bMenu, dwExStyle);
        }

        return function(lpRect, dwStyle, bMenu, dwExStyle, dpi);
#else
        return ::AdjustWindowRectExForDpi(lpRect, dwStyle, bMenu, dwExStyle, dpi);
#endif
    }

    UINT WindowsWindow::GetDpiForWindow_(HWND hwnd)
    {
#if defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
        // MinGW doesn't load the DPI-aware functions.
        using FunctionSignature = UINT (*)(HWND);

        HMODULE user32 = ::GetModuleHandleW(L"user32.dll");
        if (user32 == nullptr)
            KITSUNE_ENGINE_ERROR_("Failed to get a handle to User32.dll.");

        auto function = (FunctionSignature)(void*)(::GetProcAddress(
            user32, "GetDpiForWindow"));

        if (function == nullptr)
        {
            KITSUNE_ENGINE_ERROR_("Failed to retrieve GetDpiForWindow() from "
                                  "User32.dll. This might be because your Windows installation "
                                  "is too old. Falling back to 96 DPI.");

            return 96;
        }

        return function(hwnd);
#else
        return ::GetDpiForWindow(hwnd);
#endif
    }
}
