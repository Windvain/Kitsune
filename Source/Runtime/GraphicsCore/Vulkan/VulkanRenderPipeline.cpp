#include "GraphicsCore/Vulkan/VulkanRenderPipeline.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanShaderModule.h"

#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    namespace Details
    {
        VkPrimitiveTopology EngineToVulkan_(PrimitiveTopology topology)
        {
            switch (topology)
            {
            case PrimitiveTopology::PointList:
                return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case PrimitiveTopology::LineList:
                return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case PrimitiveTopology::LineStrip:
                return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            case PrimitiveTopology::TriangleList:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case PrimitiveTopology::TriangleStrip:
                return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            }

            KITSUNE_UNREACHABLE();
        }

        VkPolygonMode EngineToVulkan_(PolygonFillMode mode)
        {
            switch (mode)
            {
            case PolygonFillMode::Wireframe:
                return VK_POLYGON_MODE_LINE;
            case PolygonFillMode::Solid:
                return VK_POLYGON_MODE_FILL;
            }

            KITSUNE_UNREACHABLE();
        }

        VkPipeline GetVulkanHandle_(const SharedPtr<RenderPipeline>& pipeline)
        {
            return DynamicPointerCast<VulkanRenderPipeline>(
                pipeline)->GetVulkanPipeline();
        }
    }

    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
    // m_Layout and m_Pipeline are instantiated by vkCreatePipelineLayout() and
    // vkCreateGraphicsPipelines functions respectively.
    VulkanRenderPipeline::VulkanRenderPipeline(
        VulkanGpuDevice& device,
        const RenderPipelineSpecifications& specifications)
        : m_Device(device)
    {
        CreatePipelineLayout_();

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            CreateShaderStageInfo_(specifications.VertexShader,
                                   VK_SHADER_STAGE_VERTEX_BIT),
            CreateShaderStageInfo_(specifications.FragmentShader,
                                   VK_SHADER_STAGE_FRAGMENT_BIT)
        };

        auto dynamicStates = CreateDynamicStateInfo_();
        auto viewportState = CreateViewportState_(1, 1);

        auto inputAssembly = CreateInputAssemblyInfo_(specifications.Topology);
        auto vertexInputInfo = CreateVertexInputInfo_();
        auto rasterizationState = CreateRasterizationState_(specifications.FillMode);

        auto multisampleState = CreateMultisampleState_();

        auto colorBlendAttachment = CreateColorBlendAttachment_();
        auto colorBlendState = CreateColorBlendState_(&colorBlendAttachment, 1);

        VkFormat imageFormat = Details::EngineToVulkan_(
            specifications.RenderTargetFormat);

        VkPipelineRenderingCreateInfo renderingInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .pNext = nullptr,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &imageFormat,
            .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
        };

        VkGraphicsPipelineCreateInfo pipelineInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &renderingInfo,
            .flags = 0,
            .stageCount = KITSUNE_ARRAY_SIZE(shaderStages),
            .pStages = shaderStages,
            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pTessellationState = nullptr,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizationState,
            .pMultisampleState = &multisampleState,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendState,
            .pDynamicState = &dynamicStates,
            .layout = m_Layout,
            .renderPass = VK_NULL_HANDLE,    // Using dynamic rendering, no render pass.
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = 0
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateGraphicsPipelines(
                m_Device.GetVulkanDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
                nullptr, &m_Pipeline),
            "Failed to create a Vulkan graphics pipeline!");
    }
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)

    VulkanRenderPipeline::~VulkanRenderPipeline()
    {
        ::vkDestroyPipeline(m_Device.GetVulkanDevice(), m_Pipeline, nullptr);
        ::vkDestroyPipelineLayout(m_Device.GetVulkanDevice(), m_Layout, nullptr);
    }

    VkPipelineShaderStageCreateInfo VulkanRenderPipeline::CreateShaderStageInfo_(
        const SharedPtr<ShaderModule>& module,
        VkShaderStageFlagBits shaderFlag)
    {
        auto* vulkanModule = dynamic_cast<VulkanShaderModule*>(module.Get());
        KITSUNE_ASSERT(
            vulkanModule != nullptr,
            "The shader module passed in is not a Vulkan shader module.");

        const char* entryPoint;
        switch (shaderFlag)
        {
        case VK_SHADER_STAGE_VERTEX_BIT:
            entryPoint = "VertexMain";
            break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            entryPoint = "FragmentMain";
            break;
        default:
            KITSUNE_UNREACHABLE();
        }

        return VkPipelineShaderStageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = shaderFlag,
            .module = vulkanModule->GetVulkanShaderModule(),
            .pName = entryPoint,
            .pSpecializationInfo = nullptr
        };
    }

    VkPipelineDynamicStateCreateInfo VulkanRenderPipeline::CreateDynamicStateInfo_()
    {
        static VkDynamicState dynamicStateEnables[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = KITSUNE_ARRAY_SIZE(dynamicStateEnables),
            .pDynamicStates = dynamicStateEnables
        };
    }

    VkPipelineVertexInputStateCreateInfo VulkanRenderPipeline::CreateVertexInputInfo_()
    {
        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
        };
    }

    VkPipelineInputAssemblyStateCreateInfo
    VulkanRenderPipeline::CreateInputAssemblyInfo_(PrimitiveTopology topology)
    {
        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = Details::EngineToVulkan_(topology),
            .primitiveRestartEnable = VK_FALSE
        };
    }

    VkPipelineViewportStateCreateInfo VulkanRenderPipeline::CreateViewportState_(
        Uint32 viewportCount, Uint32 scissorCount)
    {
        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = viewportCount,
            .pViewports = nullptr,
            .scissorCount = scissorCount,
            .pScissors = nullptr
        };
    }

    VkPipelineRasterizationStateCreateInfo
    VulkanRenderPipeline::CreateRasterizationState_(PolygonFillMode polygonMode)
    {
        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = Details::EngineToVulkan_(polygonMode),
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f
        };
    }

    VkPipelineMultisampleStateCreateInfo VulkanRenderPipeline::CreateMultisampleState_()
    {
        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 0.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE
        };
    }

    VkPipelineColorBlendAttachmentState
    VulkanRenderPipeline::CreateColorBlendAttachment_()
    {
        return {
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };
    }

    VkPipelineColorBlendStateCreateInfo VulkanRenderPipeline::CreateColorBlendState_(
        VkPipelineColorBlendAttachmentState* attachments,
        Uint32 attachmentCount)
    {
        return {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = attachmentCount,
            .pAttachments = attachments,
            .blendConstants = { 0, 0, 0, 0 }
        };
    }

    void VulkanRenderPipeline::CreatePipelineLayout_()
    {
        VkPipelineLayoutCreateInfo layoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreatePipelineLayout(m_Device.GetVulkanDevice(), &layoutCreateInfo,
                                     nullptr, &m_Layout),
            "Failed to create a Vulkan pipeline layout.");
    }
}
