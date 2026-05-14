#pragma once

#include "Foundation/Maths/Vector2.h"

namespace Kitsune
{
    template<typename T>
    class Rect2
    {
    public:
        inline Rect2()
            : Position(), Size()
        {
        }

        inline Rect2(const Point2<T>& position, const Vector2<T>& size)
            : Position(position), Size(size)
        {
        }

    public:
        Point2<T> Position;
        Vector2<T> Size;
    };
}
