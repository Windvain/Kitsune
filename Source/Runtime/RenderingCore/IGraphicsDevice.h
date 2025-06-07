#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Utilities/NonCopyable.h"

#include "RenderingCore/ISwapChain.h"
#include "RenderingCore/IRenderTarget.h"
#include "RenderingCore/ICommandQueue.h"

#include "RenderingCore/IPhysicalDevice.h"
#include "RenderingCore/GraphicsBackend.h"

namespace Kitsune
{
    enum class GpuPreference
    {
        None,
        HighPerformance,
        PowerSaving,
    };

    struct GraphicsDeviceSpecs
    {
        bool UseDebug;
        GpuPreference GpuHint;

        GraphicsBackend Backend;
    };

    class IGraphicsDevice : public NonCopyable
    {
    public:
        virtual ~IGraphicsDevice() { /* ... */ }

    public:
        virtual SharedPtr<IPhysicalDevice> GetPhysicalDevice() const = 0;
        virtual SharedPtr<ICommandQueue> GetGraphicsCommandQueue() const = 0;

        virtual SharedPtr<ISwapChain> CreateSwapChain(const SwapChainSpecs& specs) = 0;
        virtual SharedPtr<IRenderTarget> CreateRenderTarget(const RenderTargetSpecs& specs) = 0;

    public:
        virtual GraphicsBackend GetGraphicsBackend() const = 0;
    };

    KITSUNE_API_ SharedPtr<IGraphicsDevice> CreateGraphicsDevice(const GraphicsDeviceSpecs& specs);
}
