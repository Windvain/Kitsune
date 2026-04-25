#pragma once

#include "GraphicsCore/Texture.h"

#include "Foundation/Maths/Rect2.h"
#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class CommandList;
    class RenderPipeline;

    // Contains information about a render pass.
    struct RenderingSpecifications
    {
        Rect2<Uint32> RenderArea;
    };

    // Contains information about a (texture) memory barrier.
    struct TextureBarrierDescription
    {
        TextureUsage PreviousUsage;
        TextureUsage NewUsage;
    };

    // A class which is used to allocate command lists.
    // Equivalent to VkCommandPool in Vulkan, or ID3D12CommandAllocator in DX12.
    class CommandPool : public NonCopyable
    {
    public:
        virtual ~CommandPool() = default;

    public:
        [[nodiscard]]
        virtual SharedPtr<CommandList> AllocateCommandList() = 0;
    };

    // Contains a list of commands to be sent to the GPU.
    class CommandList : public NonCopyable
    {
    public:
        virtual ~CommandList() = default;

    public:
        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual void BeginRendering(
            const SharedPtr<TextureView>& texture,
            const RenderingSpecifications& specifications) = 0;

        virtual void EndRendering() = 0;

    public:
        virtual void SetViewport(
            const Rect2<float>& viewport,
            float minimumDepth, float maximumDepth) = 0;

        virtual void SetScissor(const Rect2<Uint32>& scissorRect) = 0;

    public:
        virtual void BindRenderPipeline(const SharedPtr<RenderPipeline>& pipeline) = 0;

    public:
        virtual void Draw(Uint32 vertexCount, Uint32 instanceCount,
                          Uint32 firstVertex, Uint32 firstInstance) = 0;

        virtual void Reset() = 0;
        virtual void TextureBarrier(
            const SharedPtr<Texture>& texture,
            const TextureBarrierDescription& description) = 0;
    };
}
