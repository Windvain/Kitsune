#pragma once

#include <Windows.h>
#include "ApplicationCore/IWindow.h"

namespace Kitsune
{
    class Application;

    class WindowsWindow : public IWindow
    {
    public:
        KITSUNE_API_ WindowsWindow(int width, int height, int x, int y,
                                   const wchar_t* title, WindowState state, WindowFlag flags);

        KITSUNE_API_ ~WindowsWindow();

    public:
        [[nodiscard]] KITSUNE_API_ Vector2<Uint32> GetSize() const override;
        [[nodiscard]] KITSUNE_API_ Vector2<Int32> GetPosition() const override;

        KITSUNE_API_ void SetSize(const Vector2<Uint32>& size) override;
        KITSUNE_API_ void SetPosition(const Vector2<Int32>& pos) override;

    public:
        KITSUNE_API_ void SetTitle(const StringView title) override;

        [[nodiscard]]
        inline String GetTitle() const override
        {
            return m_Title;
        }

    public:
        KITSUNE_API_ void SetState(WindowState state) override;

        [[nodiscard]]
        KITSUNE_API_ WindowState GetState() const override;

    public:
        KITSUNE_API_ void Fullscreen() override;

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

        [[nodiscard]] inline bool IsMinimized()  const override
        {
            return (GetState() == WindowState::Minimized);
        }

        [[nodiscard]] inline bool IsMaximized()  const override
        {
            return (GetState() == WindowState::Maximized);
        }

        [[nodiscard]] inline bool IsWindowed()   const override
        {
            return (GetState() == WindowState::Windowed);
        }

    public:
        KITSUNE_API_ void Show() override;
        KITSUNE_API_ void Hide() override;

        KITSUNE_API_ bool IsShown() const override;

    private:
        KITSUNE_API_ static WNDCLASSEXW GetWindowClass();
        KITSUNE_API_ static LRESULT KitsuneWindowProc(HWND windowHandle, UINT message,
                                                      WPARAM wparam, LPARAM lparam);

        KITSUNE_API_ DWORD GetWindowStyles() const;
        KITSUNE_API_ DWORD GetExtendedWindowStyles() const;

        KITSUNE_API_ void UndoFullscreen();

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
