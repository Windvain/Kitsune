#pragma once

#include "GraphicsCore/Texture.h"

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Containers/Pair.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Semaphore;
    class CommandQueue;

    // The presentation mode of a swap chain.
    enum class PresentMode
    {
        Immediate,      //< Analogous to turning off V-Sync, can cause screen
                        //  tearing.
        Fifo,           //< Blocks the GPU from running faster than the display's
                        //  refresh rate, but avoids screen tearing.
        Mailbox         //< A combination of both Immediate and FIFO modes. Doesn't
                        //  block the GPU from running faster while avoiding screen
                        //  tearing.
    };

    // Contains settings for configuring the swap chain.
    struct SwapChainConfiguration
    {
        Uint32 ImageCount;
        Vector2<Uint32> Extent;

        TextureFormat Format;
        PresentMode PresentMode;
    };

    // A list of images to be presented to the screen.
    class SwapChain : public NonCopyable
    {
    public:
        virtual ~SwapChain() = default;

    public:
        [[nodiscard]]
        virtual SharedPtr<Texture> GetBackBuffer(Uint32 index) const = 0;

        [[nodiscard]]
        virtual Pair<Uint32, bool> AcquireNextImage(
            const SharedPtr<Semaphore>& waitedSemaphore) = 0;

    public:
        virtual void Present(Uint32 index, const SharedPtr<Semaphore>& semaphore) = 0;
        virtual void Resize(const Vector2<Uint32>& newSize) = 0;

    public:
        [[nodiscard]]
        virtual SwapChainConfiguration GetConfiguration() const = 0;

        [[nodiscard]]
        inline Uint32 GetImageCount() const
        {
            return GetConfiguration().ImageCount;
        }

        [[nodiscard]]
        inline Vector2<Uint32> GetExtent() const
        {
            return GetConfiguration().Extent;
        }

        [[nodiscard]]
        inline TextureFormat GetSurfaceFormat() const
        {
            return GetConfiguration().Format;
        }

        [[nodiscard]]
        inline PresentMode GetPresentMode() const
        {
            return GetConfiguration().PresentMode;
        }
    };
}
