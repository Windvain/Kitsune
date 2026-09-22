#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "Foundation/Utilities/EnumFlags.h"

namespace Kitsune
{
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
        Hidden = 1 << 1,
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowCreationFlags);

    struct WindowConfigurations
    {
        String Title;

        Vector2<Uint32> Size;
        Vector2<Int32> Position;

        WindowCreationFlags Flags;
        WindowState State;
    };

    class Window : public NonCopyable
    {
    public:
        virtual ~Window() = default;

    public:
        virtual void Open(const WindowConfigurations& configurations) = 0;
        virtual void Close() = 0;

        [[nodiscard]]
        virtual bool IsOpen() const = 0;

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]] virtual String GetTitle() const = 0;
        [[nodiscard]] virtual WindowState GetState() const = 0;

        [[nodiscard]] virtual bool IsVisible() const = 0;

    public:
        virtual void SetSize(const Vector2<Uint32>& size) = 0;
        virtual void SetPosition(const Vector2<Int32>& position) = 0;

        virtual void SetTitle(StringView title) = 0;
        virtual void SetState(WindowState state) = 0;

        virtual void SetVisibility(bool visible) = 0;
    };
}
