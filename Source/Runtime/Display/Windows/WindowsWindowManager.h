#pragma once

#include <Windows.h>
#undef CreateWindow         // <Windows.h> defines CreateWindow as a macro.

#include "Display/WindowManager.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Algorithms/Contains.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    // Golly gosh amazing name. WindowsWindowManager and WindowsWindow.
    class WindowsWindowManager : public WindowManager
    {
    private:
        using AdjustWindowRectExForDpiFunc = BOOL (*)(LPRECT, DWORD, BOOL, DWORD, UINT);
        using GetDPIForWindowFunc = UINT (*)(HWND);

    public:
        WindowsWindowManager();
        ~WindowsWindowManager() override;

    public:
        void Update(double delta) override;

    public:
        [[nodiscard]]
        WindowId CreateWindow(const WindowConfigurations& configs) override;

        void DestroyWindow(WindowId windowId) override;

        [[nodiscard]]
        inline bool IsWindowClosed(WindowId windowId) const override
        {
            return !Algorithms::Contains(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<WindowsWindow*>(windowId));
        }

    public:
        [[nodiscard]]
        Vector2<Uint32> GetWindowSize(WindowId windowId) const override;

        [[nodiscard]]
        Vector2<Int32> GetWindowPosition(WindowId windowId) const override;

        [[nodiscard]]
        String GetWindowTitle(WindowId windowId) const override;

        [[nodiscard]]
        WindowState GetWindowState(WindowId windowId) const override;

        [[nodiscard]]
        bool IsWindowVisible(WindowId windowId) const override;

    public:
        void SetWindowSize(WindowId windowId, const Vector2<Uint32>& size) override;
        void SetWindowPosition(
            WindowId windowId, const Vector2<Int32>& position) override;

        void SetWindowTitle(WindowId windowId, StringView title) override;
        void SetWindowState(WindowId windowId, WindowState state) override;

        void SetWindowVisibility(WindowId windowId, bool visible) override;

    private:
        [[nodiscard]] static DWORD GetWindowStyles(WindowCreationFlags flags);
        [[nodiscard]] static DWORD GetWindowExStyles();

        static LRESULT WindowProcedure(
            HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

    private:
        // TODO: Move this to a more general location. A lot of code can and will use
        // this functionality.
        template<typename FuncType>
        static FuncType LoadDLLFunction(const wchar_t* dllName, const char* funcName)
        {
            HMODULE handle = ::GetModuleHandleW(dllName);
            if (handle == nullptr)
                throw SystemException("Failed to get a handle to the DLL.");

            auto* function = reinterpret_cast<void*>(::GetProcAddress(handle, funcName));
            if (function == nullptr)
                throw SystemException("Failed to get an address for the function.");

            return reinterpret_cast<FuncType>(function);
        }

        inline void AdjustWindowRect(
            LPRECT rect, DWORD styles, BOOL menu, DWORD exStyles, UINT dpi)
        {

            if (m_AdjustWindowRectExForDpi == nullptr)
            {
#if !defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
                m_AdjustWindowRectExForDpi = ::AdjustWindowRectExForDpi;
#else
                m_AdjustWindowRectExForDpi =
                    LoadDLLFunction<AdjustWindowRectExForDpiFunc>(
                        L"user32.dll",
                        "AdjustWindowRectExForDpi");
#endif
            }

            if (!m_AdjustWindowRectExForDpi(rect, styles, menu, exStyles, dpi))
            {
                throw SystemException(
                    "Failed to adjust the window rect based on its style & extended "
                    "styles. This is probably an implementation bug due to invalid "
                    "parameters.");
            }
        }

        inline UINT GetWindowDPI(HWND handle)
        {
            if (m_GetDPIForWindow == nullptr)
            {
#if !defined(KITSUNE_COMPILER_MINGW_TOOLCHAIN)
                m_GetDPIForWindow = ::GetDpiForWindow;
#else
                m_GetDPIForWindow = LoadDLLFunction<GetDPIForWindowFunc>(
                    L"user32.dll", "GetDpiForWindow");
#endif
            }

            return m_GetDPIForWindow(handle);
        }

    private:
        static constexpr const wchar_t* s_WindowClassName = L"Kitsune Window";

    private:
        struct WindowsWindow
        {
            HWND Handle = nullptr;

            // Used for fullscreen windows.
            WINDOWPLACEMENT PrevPlacement = { /* ... */ };
            DWORD PrevStyle = 0;
            bool Fullscreen = false;

            /* Might be callbacks here in the future. */
        };

        AdjustWindowRectExForDpiFunc m_AdjustWindowRectExForDpi = nullptr;
        GetDPIForWindowFunc m_GetDPIForWindow = nullptr;

        Array<ScopedPtr<WindowsWindow>> m_Windows;
    };
}
