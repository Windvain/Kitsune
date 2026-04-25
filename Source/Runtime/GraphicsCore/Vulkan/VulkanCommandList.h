#pragma once

#include "GraphicsCore/CommandList.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]]
        VkCommandBuffer GetVulkanHandle_(const SharedPtr<CommandList>& commandList);
    }

    class VulkanGpuDevice;
    class VulkanCommandQueue;

    class VulkanCommandPool : public CommandPool
    {
    public:
        VulkanCommandPool(
            VulkanGpuDevice& device,
            const SharedPtr<VulkanCommandQueue>& queue);

        ~VulkanCommandPool() override;

    public:
        [[nodiscard]]
        SharedPtr<CommandList> AllocateCommandList() override;

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
        VulkanCommandList(VulkanGpuDevice& device, VulkanCommandPool& commandPool);
        ~VulkanCommandList() override;

    public:
        void Begin() override;
        void End() override;

        void BeginRendering(
            const SharedPtr<TextureView>& texture,
            const RenderingSpecifications& specifications) override;

        void EndRendering() override;

    public:
        void SetViewport(
            const Rect2<float>& viewport,
            float minimumDepth, float maximumDepth) override;

        void SetScissor(const Rect2<Uint32>& scissorRect) override;

    public:
        void BindRenderPipeline(const SharedPtr<RenderPipeline>& pipeline) override;

    public:
        void Draw(Uint32 vertexCount, Uint32 instanceCount,
                  Uint32 firstVertex, Uint32 firstInstance) override;

        void Reset() override;
        void TextureBarrier(
            const SharedPtr<Texture>& texture,
            const TextureBarrierDescription& description) override;

    public:
        [[nodiscard]]
        inline VkCommandBuffer GetVulkanCommandBuffer() const
        {
            return m_CommandBuffer;
        }

    private:
        [[nodiscard]]
        static VkAccessFlags2 GetAccessFlags_(TextureUsage textureUsage);

        [[nodiscard]]
        static VkPipelineStageFlags2 GetPipelineStage_(TextureUsage textureUsage);

    private:
        VulkanGpuDevice& m_Device;
        VulkanCommandPool& m_CommandPool;

        VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
    };
}
