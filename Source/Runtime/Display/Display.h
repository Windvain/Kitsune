#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class DisplayOrientation
    {
        Default,
        Rotated90,
        Rotated180,
        Rotated270
    };

    class Display : public NonCopyable
    {
    public:
        virtual ~Display() = default;

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]] virtual Uint32 GetRefreshRate() const = 0;
        [[nodiscard]] virtual float GetScaling() const = 0;

    public:
        [[nodiscard]]
        virtual DisplayOrientation GetOrientation() const = 0;

        virtual void SetOrientation(DisplayOrientation orientation) = 0;
    };
}
