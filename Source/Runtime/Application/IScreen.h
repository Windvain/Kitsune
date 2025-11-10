#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Maths/Vector2.h"

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class ScreenOrientation
    {
        Landscape,
        Portrait
    };

    class IScreen : public NonCopyable
    {
    public:
        virtual ~IScreen() { /* ... */ }

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]] virtual Uint32 GetDotsPerInch() const = 0;
        [[nodiscard]] virtual ScreenOrientation GetOrientation() const = 0;

        [[nodiscard]] virtual float GetRefreshRate() const = 0;

    public:
        virtual void SetOrientation(ScreenOrientation orientation) = 0;
    };
}
