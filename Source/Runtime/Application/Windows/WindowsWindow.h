#pragma once

#include <Windows.h>
#include "Application/IWindow.h"

namespace Kitsune
{
    class WindowsWindow : public IWindow
    {
    public:
        WindowsWindow(const wchar_t* className,
                      const Vector2<int>& size, const Vector2<int>& position,
                      const wchar_t* title, WindowFlags flags);

        ~WindowsWindow();

    public:
        [[nodiscard]] Vector2<Uint32> GetSize() const override;
        [[nodiscard]] Vector2<Int32> GetPosition() const override;

        [[nodiscard]] String GetTitle() const override;

        [[nodiscard]] Vector2<Uint32> GetSizeWithDecorations() const override;
        [[nodiscard]] Vector2<Int32> GetPositionWithDecorations() const override;

    public:
        void SetSize(const Vector2<Uint32>& size) override;
        void SetPosition(const Vector2<Int32>& position) override;

        void SetTitle(const StringView title) override;

        [[nodiscard]]
        inline bool IsResizable() const override
        {
            return ((m_WindowFlags & WindowFlags::FixedSize) == WindowFlags::None);
        }

    public:
        void Maximize() override;
        void Minimize() override;

        void Fullscreen() override;
        void Restore() override;

        inline void Show() override { ::ShowWindow(m_WindowHandle, SW_SHOW); }
        inline void Hide() override { ::ShowWindow(m_WindowHandle, SW_HIDE); }

    public:
        [[nodiscard]] inline bool IsMaximized() const override { return ::IsZoomed(m_WindowHandle); }
        [[nodiscard]] inline bool IsMinimized() const override { return ::IsIconic(m_WindowHandle); }

        [[nodiscard]] inline bool IsShown()      const override { return ::IsWindowVisible(m_WindowHandle); }
        [[nodiscard]] inline bool IsFullscreen() const override { return m_Fullscreen; }

        [[nodiscard]]
        inline bool IsWindowed() const override
        {
            return (!IsMaximized() && !IsMinimized() && !IsFullscreen());
        }

    public:
        inline HWND GetNativeHandle() const { return m_WindowHandle; }

    private:
        DWORD GetWindowStyles() const;
        DWORD GetExtendedWindowStyles() const;

    private:
        HWND m_WindowHandle;
        WindowFlags m_WindowFlags;

        bool m_Fullscreen;
        WINDOWPLACEMENT m_PrevPlacement;
    };
}
