#include "ApplicationCore/Windows/WindowsWindow.h"
#include "Foundation/Windows/StringConversions.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/Application.h"
#include "ApplicationCore/BadWindowCreationException.h"

namespace Kitsune
{
    WindowsWindow::WindowsWindow(StringView title, const Vector2<int>& size,
                                                   const Vector2<int>& pos)
        : m_Application(Application::GetInstance())
    {
        WNDCLASSEXW windowClass = GetWindowClass();
        if (::RegisterClassExW(&windowClass) == 0)
            throw BadWindowCreationException("Failed to register window class");

        DWORD exStyle = GetExtendedWindowStyles();
        DWORD style = GetWindowStyles();
        WideString wideTitle = Internal::WindowsConvertToUtf16(title);

        RECT rect = { pos.x, pos.y, size.x, size.y };
        ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());

        m_NativeHandle = ::CreateWindowExW(
            exStyle, s_WindowClassName,
            wideTitle.Raw(), style,
            rect.left, rect.top,
            rect.right - rect.left, rect.bottom - rect.top,
            nullptr, nullptr, nullptr, nullptr);

        if (m_NativeHandle == nullptr)
            throw BadWindowCreationException("Failed to create a window");

        ::SetWindowLongPtrW(m_NativeHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ::ShowWindow(m_NativeHandle, SW_SHOW);
    }

    WindowsWindow::~WindowsWindow()
    {
        ::DestroyWindow(m_NativeHandle);
    }

    void WindowsWindow::SetSize(const Vector2<Uint32>& size)
    {
        RECT rect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };
        ::AdjustWindowRectEx(&rect, GetWindowStyles(), false, GetExtendedWindowStyles());

        ::SetWindowPos(m_NativeHandle, nullptr, 0, 0,
                       rect.right - rect.left,
                       rect.bottom - rect.top, SWP_NOMOVE);
    }

    void WindowsWindow::SetPosition(const Vector2<Int32>& pos)
    {
        RECT rect = { pos.x, pos.y, static_cast<LONG>(m_Size.x), static_cast<LONG>(m_Size.y) };
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

    void WindowsWindow::Minimize() { ::ShowWindow(m_NativeHandle, SW_MINIMIZE); }
    void WindowsWindow::Maximize() { ::ShowWindow(m_NativeHandle, SW_MAXIMIZE); }
    void WindowsWindow::Restore()  { ::ShowWindow(m_NativeHandle, SW_NORMAL);   }

    void WindowsWindow::Show() { ::ShowWindow(m_NativeHandle, SW_SHOW); }
    void WindowsWindow::Hide() { ::ShowWindow(m_NativeHandle, SW_HIDE); }

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
        case WM_SIZE:
        {
            window->m_Size = { static_cast<Uint32>(LOWORD(lparam)), static_cast<Uint32>(HIWORD(lparam)) };

            switch (wparam)
            {
            case SIZE_MAXIMIZED:
            {
                RECT windowRect;
                ::GetWindowRect(windowHandle, &windowRect);
                ::AdjustWindowRectEx(&windowRect, GetWindowStyles(), false, GetExtendedWindowStyles());

                window->m_State = WindowState::Maximized;
                window->m_Position = { windowRect.left, windowRect.right };

                app->OnWindowMaximize(*window);
                break;
            }
            case SIZE_MINIMIZED:
            {
                window->m_State = WindowState::Minimized;
                app->OnWindowMinimize(*window);
                break;
            }
            default:
            {
                window->m_State = WindowState::Floating;
                app->OnWindowResize(*window, window->GetSize());

                break;
            }
            }

            break;
        }

        case WM_MOVE:
        {
            window->m_Position = { static_cast<Int32>(LOWORD(lparam)), static_cast<Int32>(HIWORD(lparam)) };
            app->OnWindowMove(*window, window->GetPosition());

            break;
        }

        case WM_CLOSE:
        {
            app->Exit(0);
            break;
        }

        case WM_SHOWWINDOW:
        {
            window->m_Visible = static_cast<BOOL>(wparam);
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
            throw BadWindowCreationException();

        return MakeScoped<WindowsWindow>(props.Title, static_cast<Vector2<int>>(props.Size),
                                         props.Position);
    }
}
