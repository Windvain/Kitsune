#pragma once

#include <Windows.h>
#include "ApplicationCore/IWindow.h"

namespace Kitsune
{
    class Application;

    class WindowsWindow : public IWindow
    {
    public:
        KITSUNE_API_ WindowsWindow(const WindowProperties& props);
        KITSUNE_API_ ~WindowsWindow();

    public:
        KITSUNE_API_ Vector2<Uint32> GetSize() const override;
        KITSUNE_API_ Vector2<Int32> GetPosition() const override;

        KITSUNE_API_ void SetSize(const Vector2<Uint32>& size) override;
        KITSUNE_API_ void SetPosition(const Vector2<Int32>& pos) override;

        KITSUNE_API_ WindowState GetState() const override { return m_State; }
        KITSUNE_API_ AABB2<Int32> GetFrameBoundingBox() const override;

        KITSUNE_API_ void SetTitle(StringView title) override;
        inline String GetTitle() const override { return m_Title; }

    public:
        KITSUNE_API_ void SetState(WindowState state) override;
        KITSUNE_API_ void Restore() override;

    public:
        KITSUNE_API_ void Show() override;
        KITSUNE_API_ void Hide() override;

        KITSUNE_API_ bool IsShown() const override;

    private:
        KITSUNE_API_ static WNDCLASSEXW GetWindowClass();
        KITSUNE_API_ static LRESULT KitsuneWindowProc(HWND windowHandle, UINT message,
                                                      WPARAM wparam, LPARAM lparam);

        KITSUNE_API_ static DWORD GetWindowStyles();
        KITSUNE_API_ static DWORD GetExtendedWindowStyles();

    private:
        static constexpr const wchar_t* s_WindowClassName = L"KitsuneWindows";
        friend LRESULT WindowsWindow::KitsuneWindowProc(HWND, UINT, WPARAM, LPARAM);

    private:
        static Uint32 s_WindowCount;

    private:
        HWND m_NativeHandle;
        Application* m_Application;

        String m_Title;
        VideoMode m_VideoMode;

        WindowState m_State;
    };
}
