#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Maths/Vector2.h"

namespace Kitsune
{
    class VideoMode
    {
    public:
        VideoMode() = default;
        VideoMode(Uint32 bpp, const Vector2<Uint32>& resolution, Uint32 refreshRateHz)
            : BitsPerPixel(bpp), Resolution(resolution), RefreshRate(refreshRateHz)
        {
        }

    public:
        Uint32 BitsPerPixel = 0;
        Vector2<Uint32> Resolution = Vector2<Uint32>();
        Uint32 RefreshRate = 0;
    };

    inline bool operator==(const VideoMode& lhs, const VideoMode& rhs)
    {
        return (lhs.BitsPerPixel == rhs.BitsPerPixel) &&
               (lhs.Resolution   == rhs.Resolution  ) &&
               (lhs.RefreshRate  == rhs.RefreshRate );
    }
}
