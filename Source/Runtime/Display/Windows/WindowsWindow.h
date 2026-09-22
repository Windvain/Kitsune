#pragma once

#include <Windows.h>
#include "Display/Window.h"

#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    class WindowsWindow : public Window
    {
    public:
        KITSUNE_API explicit WindowsWindow(const WindowConfigurations& configurations);
        KITSUNE_API ~WindowsWindow() override;

    public:
        KITSUNE_API void Open(const WindowConfigurations& configurations) override;
        KITSUNE_API void Close() override;

        [[nodiscard]]
        KITSUNE_API bool IsOpen() const override;

    public:
        [[nodiscard]] KITSUNE_API Vector2<Uint32> GetSize() const override;
        [[nodiscard]] KITSUNE_API Vector2<Int32> GetPosition() const override;

        [[nodiscard]] KITSUNE_API String GetTitle() const override;
        [[nodiscard]] KITSUNE_API WindowState GetState() const override;

        [[nodiscard]] KITSUNE_API bool IsVisible() const override;

    public:
        [[nodiscard]] KITSUNE_API bool IsUserResizable() const override;

    public:
        KITSUNE_API void SetSize(const Vector2<Uint32>& size) override;
        KITSUNE_API void SetPosition(const Vector2<Int32>& position) override;

        KITSUNE_API void SetTitle(StringView title) override;
        KITSUNE_API void SetState(WindowState state) override;

        KITSUNE_API void SetVisibility(bool visible) override;

    private:
        [[nodiscard]] static DWORD GetWindowStyles(WindowCreationFlags flags);
        [[nodiscard]] static DWORD GetWindowExtendedStyles();

        inline void VerifyWindowIsOpen() const
        {
            if (!IsOpen())
            {
                throw SystemException(
                    "This is not a valid operation to do with this object. The window "
                    "object does not own a valid handle to a window.");
            }
        }

        static UINT GetDPIForClosestMonitor(RECT* rect);
        static void AdjustRectFromStyles(
            RECT* rect, DWORD styles, DWORD extendedStyles, UINT dpi);

    private:
        HWND m_Handle = nullptr;

        // Fullscreen settings.
        DWORD m_PreviousStyle = 0;
        WINDOWPLACEMENT m_PreviousPlacement;

        bool m_Fullscreen = false;
    };
}
