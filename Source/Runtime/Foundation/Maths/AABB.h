#pragma once

#include "Foundation/Maths/VectorBase.h"

namespace Kitsune
{
    template<typename T, Usize N>
    class AABB
    {
    public:
        static_assert((N == 2) || (N == 3), "Only 2D and 3D bounding boxes are supported.");
        using PointType = VectorBase<T, N>;

    public:
        AABB() = default;
        AABB(const PointType& tl, const PointType& br)
            : TopLeft(tl), BottomRight(br)
        {
        }

    public:
        PointType TopLeft;
        PointType BottomRight;
    };

    template<typename T>
    using AABB2 = AABB<T, 2>;
}
