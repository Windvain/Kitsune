#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Utilities/NonCopyable.h"

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Maths/Fraction.h"

namespace Kitsune
{
    enum class ScreenOrientation
    {
        Landscape,
        Portrait
    };

    // Represents a display screen.
    // - GetOrientation() simply compares the width and height of the screen to determine orientation
    //   on desktop platforms. An application cannot check whether the screen is upside-down or not.
    // - SetOrientation() doesn't do anything on desktop platforms.
    class Screen : public NonCopyable
    {
    public:
        virtual ~Screen() { /* ... */ }

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]] virtual Uint32 GetDotsPerInch() const = 0;
        [[nodiscard]] virtual Fraction<Uint32> GetRefreshRate() const = 0;

        [[nodiscard]]
        inline virtual ScreenOrientation GetOrientation() const
        {
            Vector2<Uint32> size = GetSize();
            return (size.X < size.Y) ? ScreenOrientation::Portrait : ScreenOrientation::Landscape;
        }

    public:
        virtual void SetOrientation(ScreenOrientation orientation) = 0;
    };
}
