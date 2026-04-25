#pragma once

#include "GraphicsCore/Fence.h"
#include "GraphicsCore/Texture.h"
#include "GraphicsCore/Semaphore.h"

#include "GraphicsCore/CommandList.h"
#include "GraphicsCore/CommandQueue.h"
#include "GraphicsCore/ShaderModule.h"
#include "GraphicsCore/RenderPipeline.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class RenderSurface;

    // Provides access to a low-level graphics API.
    class GpuDevice : public NonCopyable
    {
    public:
        virtual ~GpuDevice() = default;

    public:
        [[nodiscard]]
        virtual SharedPtr<CommandQueue> GetQueue(
            Uint32 index, Uint32 queueIndex) const = 0;

    public:
        [[nodiscard]]
        virtual SharedPtr<Fence> CreateFence() = 0;

        // Windows defines CreateSemaphore() as a macro. Microslop you've done
        // it again!
        [[nodiscard]]
        virtual SharedPtr<Semaphore> MakeSemaphore() = 0;

    public:
        [[nodiscard]]
        virtual SharedPtr<CommandPool> CreateCommandPool(
            const SharedPtr<CommandQueue>& queue) = 0;

        [[nodiscard]]
        virtual SharedPtr<RenderPipeline> CreateRenderPipeline(
            const RenderPipelineSpecifications& specifications) = 0;

        [[nodiscard]]
        virtual SharedPtr<ShaderModule> CreateShaderModule(
            const Byte* shaderSource, Usize sourceSize) = 0;

        [[nodiscard]]
        virtual SharedPtr<TextureView> CreateTextureView(
            const SharedPtr<Texture>& texture,
            const TextureViewSpecifications& specs) = 0;
    };
}
