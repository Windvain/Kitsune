#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "Foundation/Utilities/EnumFlags.h"

namespace Kitsune
{
#if defined(KITSUNE_OS_WINDOWS)
    using WindowId = void*;
#endif

    enum class WindowState
    {
        Windowed,
        Minimized,
        Maximized,
        Fullscreen
    };

    enum class WindowCreationFlags
    {
        None = 0,
        ResizeEnabled = 1 << 0,
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowCreationFlags);

    struct WindowConfigurations
    {
        String Title;

        Vector2<Uint32> Size;
        Vector2<Int32> Position;

        WindowCreationFlags Flags = WindowCreationFlags::None;
        WindowState State = WindowState::Windowed;
    };

    class KITSUNE_API WindowManager
    {
    public:
        virtual ~WindowManager() = default;

    public:
        static WindowManager* Initialize(StringView serverName);
        static void Shutdown();

        [[nodiscard]]
        inline static WindowManager* GetInstance()
        {
            return s_Instance;
        }

    public:
        virtual void Update(double delta) = 0;

    public:
        [[nodiscard]]
        virtual WindowId CreateWindow(const WindowConfigurations& configs) = 0;

        virtual void DestroyWindow(WindowId windowId) = 0;

        [[nodiscard]]
        virtual bool IsWindowClosed(WindowId windowId) const = 0;

    public:
        [[nodiscard]]
        virtual Vector2<Uint32> GetWindowSize(WindowId windowId) const = 0;

        [[nodiscard]]
        virtual Vector2<Int32> GetWindowPosition(WindowId windowId) const = 0;

        [[nodiscard]]
        virtual String GetWindowTitle(WindowId windowId) const = 0;

        [[nodiscard]]
        virtual WindowState GetWindowState(WindowId windowId) const = 0;

        [[nodiscard]]
        virtual bool IsWindowVisible(WindowId windowId) const = 0;

    public:
        virtual void SetWindowSize(WindowId windowId, const Vector2<Uint32>& size) = 0;
        virtual void SetWindowPosition(
            WindowId windowId, const Vector2<Int32>& position) = 0;

        virtual void SetWindowTitle(WindowId windowId, StringView title) = 0;
        virtual void SetWindowState(WindowId windowId, WindowState state) = 0;

        virtual void SetWindowVisibility(WindowId windowId, bool visible) = 0;

    private:
        static WindowManager* s_Instance;
    };
}
