#pragma once

#include "Foundation/Memory/SharedPtr.h"

#include "RenderingCore/IFence.h"
#include "RenderingCore/ISwapChain.h"

#include "RenderingCore/ICommandQueue.h"
#include "RenderingCore/ICommandBuffer.h"

namespace Kitsune
{
    class ILogicalDevice
    {
    public:
        virtual ~ILogicalDevice() = default;

    public:
        virtual SharedPtr<ICommandQueue> GetCommandQueue(CommandBufferType type) = 0;

    public:
        virtual SharedPtr<ISwapChain> CreateSwapChain(const SwapChainSpecs& specs) = 0;
        virtual SharedPtr<ICommandBuffer> CreateCommandBuffer(CommandBufferType type) = 0;

        virtual SharedPtr<IFence> CreateFence(Uint64 initialValue = 0) = 0;
    };
}
