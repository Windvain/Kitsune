#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/Formatter.h"

namespace Kitsune
{
    // The screen's orientation.
    // Note: Setting a screen's orientation to 90° twice doesn't make the screen
    //       rotate 180°, a.k.a screen rotations don't stack.
    enum class ScreenOrientation
    {
        Default,
        Rotated90,
        Rotated180,
        Rotated270
    };

    // An abstract class representing a virtual screen.
    class Screen
    {
    public:
        virtual ~Screen() = default;

    public:
        [[nodiscard]]
        virtual String GetName() const = 0;

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]] virtual Uint32 GetRefreshRate() const = 0;
        [[nodiscard]] virtual Uint32 GetDotsPerInch() const = 0;
        [[nodiscard]] virtual ScreenOrientation GetOrientation() const = 0;

    public:
        [[nodiscard]]
        inline bool IsLandscapeOrientation() const
        {
            Vector2<Uint32> size = GetSize();
            return (size.X >= size.Y);
        }

        [[nodiscard]]
        inline bool IsPortraitOrientation() const
        {
            Vector2<Uint32> size = GetSize();
            return (size.X <= size.Y);
        }

    public:
        virtual void SetSize(const Vector2<Uint32>& size) = 0;
        virtual void SetOrientation(ScreenOrientation orientation) = 0;
    };

    template<>
    class Formatter<Screen, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            const Screen& screen,
            const FormatContext<Iter>& context)
        {
            Vector2<Uint32> size = screen.GetSize();
            Vector2<Uint32> position = screen.GetPosition();

            auto output = FormatTo(
                context.GetOutput(),
                "[ Name: \"{0}\", Mode: {1}x{2}@{3}Hz, Position: "
                "({4}, {5}), DPI: {6} ]",
                screen.GetName(),
                size.X, size.Y, screen.GetRefreshRate(),
                position.X, position.Y,
                screen.GetDotsPerInch());

            return output;
        }
    };

    // A handle to the `Screen` class. Defined as `Screen*`.
    using ScreenHandle = Screen*;
}
