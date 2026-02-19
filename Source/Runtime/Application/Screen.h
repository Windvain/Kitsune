#pragma once

#include "Foundation/Maths/Vector2.h"

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

    class Screen
    {
    public:
        virtual ~Screen() { /* ... */ }

    public:
        [[nodiscard]]
        virtual Vector2<Uint32> GetSize() const = 0;

        [[nodiscard]]
        virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]]
        virtual Uint32 GetRefreshRate() const = 0;

        [[nodiscard]]
        virtual Uint32 GetDotsPerInch() const = 0;

        [[nodiscard]]
        virtual ScreenOrientation GetOrientation() const = 0;

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
}
