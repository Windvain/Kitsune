#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class WindowMode
    {
        Windowed,
        Minimized,
        Maximized,

        Fullscreen,
    };

    enum class WindowFlags
    {
        None = 0,
        FullscreenPrimary = 1 << 0,      //< When fullscreen mode is enabled, the window will only
                                         //  fill the primary screen.
        ResizeDisabled = 1 << 1,         //< User input for resizing the window is disabled. Window
                                         //  size can still be modified programmatically.
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowFlags);

    class Window : public NonCopyable
    {
    public:
        virtual ~Window() { /* ... */ }

    public:
        [[nodiscard]]
        virtual Vector2<Uint32> GetSize() const = 0;

        [[nodiscard]]
        virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]]
        virtual Vector2<Uint32> GetSizeWithDecorations() const = 0;

        [[nodiscard]]
        virtual Vector2<Int32> GetPositionWithDecorations() const = 0;

    public:
        [[nodiscard]]
        virtual String GetTitle() const = 0;

        [[nodiscard]]
        virtual WindowMode GetMode() const = 0;

        [[nodiscard]]
        virtual WindowFlags GetFlags() const = 0;

    public:
        [[nodiscard]]
        virtual bool IsVisible() const = 0;

    public:
        virtual void SetSize(const Vector2<Uint32>& size) = 0;
        virtual void SetPosition(const Vector2<Int32>& position) = 0;

        virtual void SetTitle(const StringView title) = 0;
        virtual void SetMode(WindowMode mode) = 0;

    public:
        virtual void SetVisibility(bool visible) = 0;
    };
}
