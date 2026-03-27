#pragma once

#include <Windows.h>

#include "Application/Window.h"
#include "Application/DisplayManager.h"

namespace Kitsune
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(WideStringView className,
                      const WindowSpecifications& specs);

        ~WindowsWindow() override;

    public:
        [[nodiscard]] Vector2<Uint32> GetSize() const override;
        [[nodiscard]] Vector2<Int32> GetPosition() const override;

        [[nodiscard]]
        Vector2<Uint32> GetSizeWithDecorations() const override;

        [[nodiscard]]
        Vector2<Int32> GetPositionWithDecorations() const override;

    public:
        [[nodiscard]] String GetTitle() const override;
        [[nodiscard]] WindowMode GetMode() const override;

        [[nodiscard]]
        inline WindowFlags GetFlags() const override
        {
            return m_Flags;
        }

    public:
        [[nodiscard]]
        bool IsVisible() const override
        {
            return ::IsWindowVisible(m_Handle);
        }

    public:
        void SetSize(const Vector2<Uint32>& size) override;
        void SetPosition(const Vector2<Int32>& position) override;

        void SetTitle(StringView title) override;
        void SetMode(WindowMode mode) override;

    public:
        void SetVisibility(bool visible) override;

    public:
        [[nodiscard]]
        inline HWND GetNativeHandle() const
        {
            return m_Handle;
        }

    private:
        [[nodiscard]] static DWORD CalculateWindowStyles_(WindowFlags flags);
        [[nodiscard]] static DWORD CalculateWindowExStyles_(WindowFlags flags);

        void EnableFullscreen_();
        void DisableFullscreen_();

    private:
        static BOOL AdjustWindowRectExForDpi_(
            LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
            DWORD dwExStyle, UINT dpi);

        [[nodiscard]]
        static UINT GetDpiForWindow_(HWND hwnd);

    private:
        HWND m_Handle;
        WindowFlags m_Flags;

        // Fullscreen-specific variables.
        bool m_Fullscreen = false;
        WINDOWPLACEMENT m_PreviousPlacement = { /* ... */ };
    };
}
