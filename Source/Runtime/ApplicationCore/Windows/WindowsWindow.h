#pragma once

#include <Windows.h>
#include "ApplicationCore/IWindow.h"

namespace Kitsune
{
    class Application;

    class WindowsWindow : public IWindow
    {
    public:
        WindowsWindow(int width, int height, int x, int y,
                                   const wchar_t* title, WindowState state, WindowFlag flags);

        ~WindowsWindow();

    public:
        [[nodiscard]] Vector2<Uint32> GetSize() const override;
        [[nodiscard]] Vector2<Int32> GetPosition() const override;

        void SetSize(const Vector2<Uint32>& size) override;
        void SetPosition(const Vector2<Int32>& pos) override;

    public:
        void SetTitle(const StringView title) override;

        [[nodiscard]]
        inline String GetTitle() const override
        {
            return m_Title;
        }

    public:
        void SetState(WindowState state) override;

        [[nodiscard]]
        WindowState GetState() const override;

    public:
        void Fullscreen() override;

        [[nodiscard]]
        inline bool IsFullscreen() const override
        {
            return (GetState() == WindowState::Fullscreen);
        }

    public:
        inline void Minimize() override
        {
            return SetState(WindowState::Minimized);
        }

        inline void Maximize() override
        {
            return SetState(WindowState::Maximized);
        }

        inline void Restore() override
        {
            return SetState(WindowState::Windowed);
        }

        [[nodiscard]] inline bool IsMinimized() const override
        {
            return (GetState() == WindowState::Minimized);
        }

        [[nodiscard]] inline bool IsMaximized() const override
        {
            return (GetState() == WindowState::Maximized);
        }

        [[nodiscard]] inline bool IsWindowed() const override
        {
            return (GetState() == WindowState::Windowed);
        }

    public:
        void Show() override;
        void Hide() override;

        bool IsShown() const override;

    public:
        inline HWND GetWindowsHandle() const { return m_NativeHandle; }

    private:
        static WNDCLASSEXW GetWindowClass();
        static LRESULT KitsuneWindowProc(HWND windowHandle, UINT message,
                                                      WPARAM wparam, LPARAM lparam);

        DWORD GetWindowStyles() const;
        DWORD GetExtendedWindowStyles() const;

        void UndoFullscreen();

    private:
        static constexpr const wchar_t* s_WindowClassName = L"Kitsune_WindowClass";

    private:
        static volatile Int32 s_WindowCount;

    private:
        String m_Title;
        HWND m_NativeHandle;

        bool m_Fullscreen;
        WINDOWPLACEMENT m_FullscreenPrev;

        WindowFlag m_WindowFlags;
        RECT m_UnresizableRect;
    };
}
