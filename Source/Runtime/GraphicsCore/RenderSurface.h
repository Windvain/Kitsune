#pragma once

#include "GraphicsCore/Texture.h"

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class GpuDevice;
    class Semaphore;

    // The presentation mode of a surface.
    enum class SurfacePresentMode
    {
        Immediate,      //< Analogous to setting V-Sync to off, can cause screen
                        //  tearing.
        Fifo,           //< Blocks the GPU from running faster than the display's
                        //  refresh rate, but avoids screen tearing.
        Mailbox         //< A combination of both Immediate and FIFO modes. Doesn't
                        //  block the GPU from running faster while avoiding screen
                        //  tearing.
    };

    // Stores the capabilities of a render surface.
    struct RenderSurfaceCapabilities
    {
        Uint32 MinimumImageCount;
        Uint32 MaximumImageCount;

        Array<SurfacePresentMode> PresentModes;
        Array<TextureFormat> TextureFormats;

        Vector2<Uint32> MinimumExtents;
        Vector2<Uint32> MaximumExtents;
    };

    // Contains settings for configuring the swap chain.
    struct SwapChainConfiguration
    {
        Uint32 ImageCount;
        Vector2<Uint32> Extents;

        TextureFormat Format;
        SurfacePresentMode PresentMode;
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

        [[nodiscard]]
        virtual SwapChainConfiguration GetSwapChainConfiguration() const = 0;

        [[nodiscard]]
        virtual SharedPtr<Texture> GetBackBuffer(Uint32 index) const = 0;

    public:
        [[nodiscard]]
        virtual Uint32 AcquireNextImage(SharedPtr<Semaphore>& semaphore) = 0;

    public:
        virtual void ConfigureSwapChain(
            const SharedPtr<GpuDevice>& device,
            const SwapChainConfiguration& configuration) = 0;
    };
}
