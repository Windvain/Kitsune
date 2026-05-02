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

        [[nodiscard]]
        VkPolygonMode ToVkPolygonMode_(PolygonFillMode fillMode);

        [[nodiscard]] VkCullModeFlags ToVkCullMode_(CullMode cullMode);
        [[nodiscard]] VkFrontFace ToVkFrontFace_(FrontFace frontFace);

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
        static VkPipelineShaderStageCreateInfo CreateShaderStageInfo_(
            const SharedPtr<ShaderModule>& module,
            VkShaderStageFlagBits shaderFlag);

    private:
        VulkanGpuDevice& m_Device;

        VkPipelineLayout m_Layout = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
    };
}
