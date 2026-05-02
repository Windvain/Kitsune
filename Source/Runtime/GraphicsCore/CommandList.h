#pragma once

#include "GraphicsCore/Texture.h"

#include "Foundation/Maths/Rect2.h"
#include "Foundation/Maths/Color4.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class CommandList;
    class RenderPipeline;

    // Specifies the level of a command list.
    enum class CommandListLevel
    {
        Primary,
        Secondary
    };

    // Contains information about a render pass.
    struct RenderingInformation
    {
        Rect2<Uint32> RenderArea;
        Color4<float> ClearColor;
    };

    // Contains information about a (texture) memory barrier.
    struct TextureMemoryBarrierDescription
    {
        SharedPtr<Texture> Texture;

        TextureLayout OldLayout;
        TextureLayout NewLayout;
    };

    // A class which is used to allocate command lists.
    // Equivalent to VkCommandPool in Vulkan, or ID3D12CommandAllocator in DX12.
    class CommandPool : public NonCopyable
    {
    public:
        virtual ~CommandPool() = default;

    public:
        [[nodiscard]]
        virtual SharedPtr<CommandList> AllocateCommandList(CommandListLevel level) = 0;
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
            const SharedPtr<TextureView>& textureView,
            const RenderingInformation& information) = 0;

        virtual void EndRendering() = 0;

    public:
        virtual void SetViewport(
            const Rect2<float>& viewport,
            float minimumDepth,
            float maximumDepth) = 0;

        virtual void SetScissor(const Rect2<Uint32>& scissorRect) = 0;

    public:
        virtual void BindRenderPipeline(const SharedPtr<RenderPipeline>& pipeline) = 0;

    public:
        virtual void Draw(Uint32 vertexCount, Uint32 instanceCount,
                          Uint32 firstVertex, Uint32 firstInstance) = 0;

        virtual void Reset() = 0;

        virtual void TextureMemoryBarrier(
            const Array<TextureMemoryBarrierDescription>& descriptions) = 0;
    };
}
