#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/Format.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // The mode of the window.
    enum class WindowMode
    {
        Windowed,
        Minimized,
        Maximized,

        Fullscreen,
    };

    // Flags specifying the characteristics of a window.
    enum class WindowFlags
    {
        None = 0,                   //< No flags are specified.
        ResizeDisabled = 1 << 0,    //< User input for resizing the window is
                                    //  disabled, but window size can still be set
                                    //  programmatically.
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowFlags);

    // An abstract class representing a platform-independent implementation of a window.
    class Window : public NonCopyable
    {
    public:
        virtual ~Window() = default;

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]]
        virtual Vector2<Uint32> GetSizeWithDecorations() const = 0;

        [[nodiscard]]
        virtual Vector2<Int32> GetPositionWithDecorations() const = 0;

    public:
        [[nodiscard]] virtual String GetTitle() const = 0;
        [[nodiscard]] virtual WindowMode GetMode() const = 0;
        [[nodiscard]] virtual WindowFlags GetFlags() const = 0;

    public:
        [[nodiscard]]
        virtual bool IsVisible() const = 0;

    public:
        virtual void SetSize(const Vector2<Uint32>& size) = 0;
        virtual void SetPosition(const Vector2<Int32>& position) = 0;

        virtual void SetTitle(StringView title) = 0;
        virtual void SetMode(WindowMode mode) = 0;

    public:
        virtual void SetVisibility(bool visible) = 0;
    };

    template<>
    class Formatter<Window, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            const Window& window,
            const FormatContext<Iter>& context)
        {
            Vector2<Uint32> size = window.GetSize();
            Vector2<Int32> position = window.GetPosition();

            auto output = FormatTo(
                context.GetOutput(),
                "[ Title: \"{0}\", Size: {1}x{2}, Position: ({3}, {4}) ]",
                window.GetTitle(),
                size.X, size.Y,
                position.X, position.Y);

            return output;
        }
    };

    // A handle to the `Window` class. Defined as `Window*`.
    using WindowHandle = Window*;
}
