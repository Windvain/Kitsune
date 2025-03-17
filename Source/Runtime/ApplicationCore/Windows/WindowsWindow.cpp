#include "ApplicationCore/Windows/WindowsWindow.h"
#include "Foundation/Windows/StringConversions.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/Application.h"
#include "ApplicationCore/BadWindowCreationException.h"

// Copied from Windows.h, just made it work with signed integers instead.
#define KITSUNE_SIGNED_LOWORD_(lparam) static_cast<INT16>(reinterpret_cast<LONG_PTR>(lparam) & 0xFFFF)
#define KITSUNE_SIGNED_HIWORD_(lparam) static_cast<INT16>((reinterpret_cast<LONG_PTR>(lparam) >> 16) & 0xFFFF)

namespace Kitsune
{
    Uint32 WindowsWindow::s_WindowCount = 0;

    WindowsWindow::WindowsWindow(const WindowProperties& props)
        : m_Application(Application::GetInstance())
    {
        KITSUNE_ASSERT(m_Application != nullptr, "Application has not been instanced.");

        WNDCLASSEXW windowClass = GetWindowClass();
        if ((s_WindowCount == 0) && (::RegisterClassExW(&windowClass) == 0))
            throw BadWindowCreationException("Failed to register window class");

        DWORD exStyle = GetExtendedWindowStyles();
        DWORD style = GetWindowStyles();
        WideString wideTitle = Internal::WindowsConvertToUtf16(props.Title);

        Vector2<Int32> pos;
        Vector2<Uint32> size;

        if (props.PositionHint == WindowPositionHint::UsePosition)
        {
            RECT rect = { props.Position.x, props.Position.y,
                          static_cast<LONG>(props.Position.x + props.Size.x),
                          static_cast<LONG>(props.Position.y + props.Size.y) };

            ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());

            pos = { rect.left, rect.top };
            size = { static_cast<Uint32>(rect.right - rect.left),
                     static_cast<Uint32>(rect.bottom - rect.top) };
        }
        else
        {
            RECT rect = { 0, 0, static_cast<LONG>(props.Size.x), static_cast<LONG>(props.Size.y) };
            ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());

            size = { static_cast<Uint32>(rect.right - rect.left),
                     static_cast<Uint32>(rect.bottom - rect.top) };

            if (props.PositionHint == WindowPositionHint::DefaultPosition)
                pos = { CW_USEDEFAULT, CW_USEDEFAULT };
            else if (props.PositionHint == WindowPositionHint::ScreenCenter)
            {
                pos = (props.VideoMode.Resolution / 2) - (size / 2);
            }
        }

        m_NativeHandle = ::CreateWindowExW(
            exStyle, s_WindowClassName,
            wideTitle.Raw(), style,
            pos.x, pos.y,
            size.x, size.y,
            nullptr, nullptr, nullptr, nullptr);

        if (m_NativeHandle == nullptr)
            throw BadWindowCreationException("Failed to create a window");

        m_VideoMode = props.VideoMode;
        ::SetWindowLongPtrW(m_NativeHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        int showWindowFlags = SW_SHOW;
        if (props.WindowState == WindowState::Minimized)
            showWindowFlags = SW_SHOWMINIMIZED;
        else if (props.WindowState == WindowState::Maximized)
            showWindowFlags = SW_SHOWMAXIMIZED;

        ::ShowWindow(m_NativeHandle, showWindowFlags);
        ++s_WindowCount;
    }

    WindowsWindow::~WindowsWindow()
    {
        ::DestroyWindow(m_NativeHandle);

        if (s_WindowCount == 1)
            ::UnregisterClassW(s_WindowClassName, nullptr);
        else
            --s_WindowCount;
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
        if (!IsFloating()) Restore();

        RECT rect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };
        ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());

        ::SetWindowPos(m_NativeHandle, nullptr, 0, 0,
                       rect.right - rect.left,
                       rect.bottom - rect.top, SWP_NOMOVE);
    }

    void WindowsWindow::SetPosition(const Vector2<Int32>& pos)
    {
        if (!IsFloating()) Restore();

        auto size = static_cast<Vector2<LONG>>(GetSize());
        RECT rect = { pos.x, pos.y, size.x, size.y };

        ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());
        ::SetWindowPos(m_NativeHandle, nullptr, rect.left, rect.top, 0, 0, SWP_NOSIZE);
    }

    AABB2<Int32> WindowsWindow::GetFrameBoundingBox() const
    {
        RECT rect;
        ::GetWindowRect(m_NativeHandle, &rect);

        return AABB2<Int32>({ rect.left, rect.top }, { rect.right, rect.bottom });
    }

    void WindowsWindow::SetTitle(StringView title)
    {
        WideString wideTitle = Internal::WindowsConvertToUtf16(title);
        ::SetWindowTextW(m_NativeHandle, wideTitle.Raw());

        m_Title = title;
    }

    void WindowsWindow::SetState(WindowState state)
    {
        DWORD showCmd = (state == WindowState::Minimized) ? SW_MINIMIZE :
                        (state == WindowState::Maximized) ? SW_MAXIMIZE :
                                                            SW_NORMAL;

        if (IsShown() || (showCmd != SW_MAXIMIZE))
            ::ShowWindow(m_NativeHandle, showCmd);
        else
        {
            // There is currently no way of maximizing
            // Maximize by setting pos & size instead, no other option.
            // HACK: Set style, then set position.
            DWORD style = ::GetWindowLongW(m_NativeHandle, GWL_STYLE);
            style |= WS_MAXIMIZE;
            ::SetWindowLongW(m_NativeHandle, GWL_STYLE, style);

            Vector2<Uint32> res = m_VideoMode.Resolution;
            ::SetWindowPos(m_NativeHandle, nullptr, 0, 0, res.x, res.y,
                           SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
        }
    }

    void WindowsWindow::Restore()
    {
        ::ShowWindow(m_NativeHandle, SW_NORMAL);
    }

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
        auto* window = reinterpret_cast<WindowsWindow*>(::GetWindowLongPtrW(windowHandle, GWLP_USERDATA));
        if (window == nullptr)      // ::CreateWindowExW() calls the window procedure with WM_CREATE.
            return ::DefWindowProcW(windowHandle, message, wparam, lparam);

        Application* app = window->m_Application;
        switch (message)
        {
        case WM_CLOSE:
        {
            SharedPtr<IWindow> primaryWindow = app->GetPrimaryWindow();
            if (primaryWindow.Get() == window)
                app->Exit(app->GetExitCode());

            break;
        }

        case WM_MOVE:
        {
            // LOWORD() and HIWORD() underflow when dealing with negative position values.
            Vector2<Int32> position = { static_cast<Int32>(KITSUNE_SIGNED_LOWORD_(lparam)),
                                        static_cast<Int32>(KITSUNE_SIGNED_HIWORD_(lparam)) };

            app->OnWindowMove(*window, position);
            break;
        }

        case WM_SIZE:
        {
            window->m_State = (wparam == SIZE_MINIMIZED) ? WindowState::Minimized :
                              (wparam == SIZE_MAXIMIZED) ? WindowState::Maximized :
                                                           WindowState::Floating;

            Vector2<Uint32> size = { static_cast<Uint32>(LOWORD(lparam)),
                                     static_cast<Uint32>(HIWORD(lparam)) };

            if (window->m_State == WindowState::Maximized)
                app->OnWindowMaximize(*window);
            else if (window->m_State == WindowState::Minimized)
                app->OnWindowMinimize(*window);

            app->OnWindowResize(*window, size);
            break;
        }

        default:
            return ::DefWindowProcW(windowHandle, message, wparam, lparam);
        }

        return 0;
    }

    DWORD WindowsWindow::GetExtendedWindowStyles()
    {
        return WS_EX_APPWINDOW;
    }

    DWORD WindowsWindow::GetWindowStyles()
    {
        return WS_OVERLAPPEDWINDOW;
    }

    SharedPtr<IWindow> MakeWindow(const WindowProperties& props)
    {
        if (props.Size == Vector2<Uint32>(0, 0))
            throw BadWindowCreationException("Cannot create a window with a size of [0, 0]");

        WindowProperties modifiedProps = props;

        if (props.VideoMode != VideoMode())
            modifiedProps.VideoMode = props.VideoMode;
        else
        {
            DEVMODEW devMode;
            devMode.dmSize = sizeof(devMode);
            devMode.dmDriverExtra = 0;

            if (::EnumDisplaySettingsW(nullptr, ENUM_CURRENT_SETTINGS, &devMode) == 0)
                throw BadWindowCreationException("Failed to get the current display's settings");

            modifiedProps.VideoMode = VideoMode(devMode.dmBitsPerPel,
                                                Vector2<Uint32>(devMode.dmPelsWidth, devMode.dmPelsHeight),
                                                devMode.dmDisplayFrequency);
        }

        return MakeScoped<WindowsWindow>(modifiedProps);
    }
}
