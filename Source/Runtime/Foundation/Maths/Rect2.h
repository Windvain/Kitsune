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

        inline Rect2(const Vector2<T>& position, const Vector2<T>& size)
            : Position(position), Size(size)
        {
        }

    public:
        Vector2<T> Position;
        Vector2<T> Size;
    };
}
