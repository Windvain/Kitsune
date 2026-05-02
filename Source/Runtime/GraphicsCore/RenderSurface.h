#pragma once

#include "GraphicsCore/Texture.h"
#include "GraphicsCore/SwapChain.h"

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Semaphore;
    class GpuDevice;

    // Stores the capabilities of a render surface.
    struct RenderSurfaceCapabilities
    {
        Uint32 MinimumImageCount;
        Uint32 MaximumImageCount;

        Array<PresentMode> PresentModes;
        Array<TextureFormat> TextureFormats;

        Vector2<Uint32> MinimumExtents;
        Vector2<Uint32> MaximumExtents;
    };

    // Contains pixel data for rendering to a window.
    class RenderSurface : public NonCopyable
    {
    public:
        virtual ~RenderSurface() = default;

    public:
        [[nodiscard]]
        virtual RenderSurfaceCapabilities GetCapabilities(
            const SharedPtr<GpuDevice>& device) const = 0;
    };
}
