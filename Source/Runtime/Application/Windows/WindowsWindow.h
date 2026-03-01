#pragma once

#include <Windows.h>
#include "Application/Window.h"

namespace Kitsune
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(const WideStringView className,
                      const Vector2<int>& size,
                      const Vector2<int>& position,
                      const WideStringView title,
                      WindowMode mode,
                      WindowFlags flags);

        ~WindowsWindow();

    public:
        [[nodiscard]]
        Vector2<Uint32> GetSize() const override;

        [[nodiscard]]
        Vector2<Int32> GetPosition() const override;

        [[nodiscard]]
        Vector2<Uint32> GetSizeWithDecorations() const override;

        [[nodiscard]]
        Vector2<Int32> GetPositionWithDecorations() const override;

    public:
        [[nodiscard]]
        String GetTitle() const override;

        [[nodiscard]]
        WindowMode GetMode() const override;

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

        void SetTitle(const StringView title) override;
        void SetMode(WindowMode mode) override;

    public:
        void SetVisibility(bool visible) override;

    private:
        RECT GetEntireVirtualScreenRect() const;

        static DWORD GetWindowStyles(WindowFlags flags);
        static DWORD GetWindowExStyles();

    private:
        static BOOL AdjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu,
                                             DWORD dwExStyle, UINT dpi);

        static UINT GetDpiForWindow(HWND hwnd);

    private:
        HWND m_Handle;
        WindowFlags m_Flags;

        // Fullscreen-specific variables.
        bool m_Fullscreen = false;
        WINDOWPLACEMENT m_PreviousPlacement;
    };
}
