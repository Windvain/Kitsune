#pragma once

#include "GraphicsCore/RenderPipeline.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanGpuDevice;
    class VulkanRenderPipeline;

    namespace Details
    {
        [[nodiscard]]
        VkPrimitiveTopology ToVkPrimitiveTopology_(PrimitiveTopology topology);

        [[nodiscard]] VkCullModeFlags ToVkCullMode_(CullMode cullMode);
        [[nodiscard]] VkFrontFace ToVkFrontFace_(FrontFace frontFace);
        [[nodiscard]] VkPolygonMode ToVkPolygonMode_(PolygonFillMode fillMode);

        [[nodiscard]] VkVertexInputRate ToVkVertexInputRate_(VertexInputRate inputRate);
        [[nodiscard]] VkFormat ToVkFormat_(VertexType vertexType);

        [[nodiscard]]
        SharedPtr<VulkanRenderPipeline> ToImplementation_(
            const SharedPtr<RenderPipeline>& pipeline);
    }

    class VulkanRenderPipeline : public RenderPipeline
    {
    public:
        VulkanRenderPipeline(
            VulkanGpuDevice& device,
            const RenderPipelineSpecifications& specifications);

        ~VulkanRenderPipeline() override;

    public:
        [[nodiscard]]
        inline VkPipeline GetVulkanPipeline() const
        {
            return m_Pipeline;
        }

    public:
        // Pipeline layouts have not yet been implemented, make dummy ones
        // instead. TODO!
        void CreateDummyLayout_();
        void DestroyDummyLayout_();

        // Helper function for creating shader stages.
        [[nodiscard]]
        static VkPipelineShaderStageCreateInfo CreateShaderStageInfo_(
            const SharedPtr<ShaderModule>& module,
            VkShaderStageFlagBits shaderFlag);

        [[nodiscard]]
        static Array<VkVertexInputBindingDescription> GetVertexBindings_(
            const Array<VertexBindingDescription>& descriptions);

        [[nodiscard]]
        static Array<VkVertexInputAttributeDescription> GetVertexAttributes_(
            const Array<VertexAttributeDescription>& descriptions);

    private:
        VulkanGpuDevice& m_Device;

        VkPipelineLayout m_Layout = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
    };
}
