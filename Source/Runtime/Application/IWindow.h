#pragma once

#include "Foundation/Common/Types.h"

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "Foundation/Utilities/EnumFlags.h"

namespace Kitsune
{
    enum class WindowPositionHint
    {
        UsePosition,            // Use the position argument.
        PrimaryScreenCenter     // Ignores the position argument and spawns the window
                                // in the center of the primary screen.
    };

    enum class WindowFlags
    {
        None = 0,               // None.
        FixedSize = 1 << 0      // The window cannot be resized.
    };

    struct WindowSpecifications
    {
        Vector2<Uint32> Size;
        String Title;

        Vector2<Int32> Position;
        WindowPositionHint PositionHint = WindowPositionHint::UsePosition;

        WindowFlags Flags;
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowFlags);

    class IWindow
    {
    public:
        virtual ~IWindow() { /* ... */ }

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]] virtual String GetTitle() const = 0;

        [[nodiscard]] virtual Vector2<Uint32> GetSizeWithDecorations() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPositionWithDecorations() const = 0;

    public:
        virtual void SetSize(const Vector2<Uint32>& size) = 0;
        virtual void SetPosition(const Vector2<Int32>& position) = 0;

        virtual void SetTitle(const StringView title) = 0;

        [[nodiscard]] virtual bool IsResizable() const = 0;

    public:
        virtual void Maximize() = 0;
        virtual void Minimize() = 0;

        virtual void Fullscreen() = 0;
        virtual void Restore() = 0;

        virtual void Show() = 0;
        virtual void Hide() = 0;

    public:
        [[nodiscard]] virtual bool IsMaximized() const = 0;
        [[nodiscard]] virtual bool IsMinimized() const = 0;

        [[nodiscard]] virtual bool IsWindowed() const = 0;
        [[nodiscard]] virtual bool IsFullscreen() const = 0;

        [[nodiscard]] virtual bool IsShown() const = 0;
    };
}
