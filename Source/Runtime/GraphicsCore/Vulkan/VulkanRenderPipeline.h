#pragma once

#include "GraphicsCore/RenderPipeline.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]] VkPrimitiveTopology EngineToVulkan_(PrimitiveTopology topology);
        [[nodiscard]] VkPolygonMode EngineToVulkan_(PolygonFillMode fillMode);

        [[nodiscard]]
        VkPipeline GetVulkanHandle_(const SharedPtr<RenderPipeline>& pipeline);
    }

    class VulkanGpuDevice;

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

    private:
        [[nodiscard]]
        static VkPipelineShaderStageCreateInfo CreateShaderStageInfo_(
            const SharedPtr<ShaderModule>& module,
            VkShaderStageFlagBits shaderFlag);

        [[nodiscard]]
        static VkPipelineDynamicStateCreateInfo CreateDynamicStateInfo_();

        [[nodiscard]]
        static VkPipelineVertexInputStateCreateInfo CreateVertexInputInfo_();

        [[nodiscard]]
        static VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyInfo_(
            PrimitiveTopology topology);

        [[nodiscard]]
        static VkPipelineViewportStateCreateInfo CreateViewportState_(
            Uint32 viewportCount, Uint32 scissorCount);

        [[nodiscard]]
        static VkPipelineRasterizationStateCreateInfo CreateRasterizationState_(
            PolygonFillMode polygonMode);

        [[nodiscard]]
        static VkPipelineMultisampleStateCreateInfo CreateMultisampleState_();

        [[nodiscard]]
        static VkPipelineColorBlendAttachmentState CreateColorBlendAttachment_();

        [[nodiscard]]
        static VkPipelineColorBlendStateCreateInfo CreateColorBlendState_(
            VkPipelineColorBlendAttachmentState* attachments,
            Uint32 attachmentCount);

    private:
        void CreatePipelineLayout_();

    private:
        VulkanGpuDevice& m_Device;

        VkPipelineLayout m_Layout;
        VkPipeline m_Pipeline;
    };
}
