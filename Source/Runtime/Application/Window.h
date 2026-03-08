#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/Format.h"

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
        ResizeDisabled = 1 << 0,         //< User input for resizing the window is disabled. Window
                                         //  size can still be modified programmatically, but the mode
                                         //  of the window can only be set to Windowed or Minimized.
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowFlags);

    // An abstract class representing a platform-independent implementation of a window.
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

    template<>
    class Formatter<Window, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(const Window& window, const FormatContext<Iter>& context)
        {
            Vector2<Uint32> size = window.GetSize();
            Vector2<Int32> position = window.GetPosition();

            auto output = FormatTo(
                context.GetOutput(),
                "Window {0}: \"{1}\" (Size: {2} x {3}, Position: [{4}, {5}])",
                AddressOf(window),
                window.GetTitle(),
                size.X, size.Y,
                position.X, position.Y);

            return output;
        }
    };

    // A handle to the `Window` class. Defined as `Window*`.
    using WindowHandle = Window*;
}
