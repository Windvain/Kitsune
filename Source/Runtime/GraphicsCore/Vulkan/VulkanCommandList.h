#pragma once

#include "GraphicsCore/CommandList.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanGpuDevice;

    class VulkanCommandList;
    class VulkanCommandQueue;

    namespace Details
    {
        [[nodiscard]]
        VkCommandBufferLevel ToVkCommandBufferLevel_(CommandListLevel level);

        [[nodiscard]]
        SharedPtr<VulkanCommandList> ToImplementation_(
            const SharedPtr<CommandList>& commandList);
    }

    class VulkanCommandPool : public CommandPool
    {
    public:
        VulkanCommandPool(
            VulkanGpuDevice& device,
            const SharedPtr<VulkanCommandQueue>& queue);

        ~VulkanCommandPool() override;

    public:
        [[nodiscard]]
        SharedPtr<CommandList> AllocateCommandList(CommandListLevel level) override;

    public:
        [[nodiscard]]
        inline VkCommandPool GetVulkanPool() const
        {
            return m_CommandPool;
        }

    private:
        VulkanGpuDevice& m_Device;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
    };

    class VulkanCommandList : public CommandList
    {
    public:
        VulkanCommandList(
            VulkanGpuDevice& device,
            VulkanCommandPool& commandPool,
            CommandListLevel level);

        ~VulkanCommandList() override;

    public:
        void Begin() override;
        void End() override;

        void BeginRendering(
            const SharedPtr<TextureView>& textureView,
            const RenderingInformation& information) override;

        void EndRendering() override;

    public:
        void SetViewport(
            const Rect2<float>& viewport,
            float minimumDepth,
            float maximumDepth) override;

        void SetScissor(const Rect2<Uint32>& scissorRect) override;

    public:
        void BindRenderPipeline(const SharedPtr<RenderPipeline>& pipeline) override;

        void BindVertexBuffers(const Array<SharedPtr<GpuBuffer>>& buffers) override;
        void BindIndexBuffer(const SharedPtr<GpuBuffer>& buffer) override;

    public:
        void CopyBuffer(const SharedPtr<GpuBuffer>& destination,
                        const SharedPtr<GpuBuffer>& source,
                        Uint64 bytes) override;

        void Draw(Uint32 vertexCount, Uint32 instanceCount) override;
        void DrawIndexed(Uint32 indexCount, Uint32 instanceCount) override;

        void Reset() override;

        void TextureMemoryBarrier(
            const Array<TextureMemoryBarrierDescription>& descriptions) override;

    public:
        [[nodiscard]]
        inline VkCommandBuffer GetVulkanCommandBuffer() const
        {
            return m_CommandBuffer;
        }

    private:
        [[nodiscard]]
        static VkPipelineStageFlags2 GetPipelineStage_(TextureLayout layout);

        [[nodiscard]]
        static VkAccessFlags2 GetAccessFlags_(TextureLayout layout);

    private:
        VulkanGpuDevice& m_Device;
        VulkanCommandPool& m_CommandPool;

        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    };
}
