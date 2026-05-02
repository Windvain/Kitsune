#include "GraphicsCore/Vulkan/VulkanRenderPipeline.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanShaderModule.h"

#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    namespace Details
    {
        VkPrimitiveTopology ToVkPrimitiveTopology_(PrimitiveTopology topology)
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

        VkPolygonMode ToVkPolygonMode_(PolygonFillMode mode)
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

        VkCullModeFlags ToVkCullMode_(CullMode cullMode)
        {
            switch (cullMode)
            {
            case CullMode::None:
                return { /* ... */ };
            case CullMode::Front:
                return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Back:
                return VK_CULL_MODE_BACK_BIT;
            }

            KITSUNE_UNREACHABLE();
        }

        VkFrontFace ToVkFrontFace_(FrontFace frontFace)
        {
            switch (frontFace)
            {
            case FrontFace::Clockwise:
                return VK_FRONT_FACE_CLOCKWISE;
            case FrontFace::CounterClockwise:
                return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            }

            KITSUNE_UNREACHABLE();
        }

        SharedPtr<VulkanRenderPipeline> ToImplementation_(
            const SharedPtr<RenderPipeline>& pipeline)
        {
            return DynamicPointerCast<VulkanRenderPipeline>(pipeline);
        }
    }

    VulkanRenderPipeline::VulkanRenderPipeline(
        VulkanGpuDevice& device,
        const RenderPipelineSpecifications& specifications)
        : m_Device(device)
    {
        // TODO: Implement a PipelineLayout class.
        CreateDummyLayout_();

        if ((specifications.FillMode == PolygonFillMode::Wireframe) &&
            !bool(device.GetFeatures() & GpuDeviceFeature::WireframeRendering))
        {
            throw InvalidArgumentException(
                "Failed to create a Vulkan render pipeline. PolygonFillMode::Wireframe "
                "was defined without enabling GpuDeviceFeature::WireframeRendering.");
        }

        VkPipelineShaderStageCreateInfo shaderStages[] = {
            CreateShaderStageInfo_(specifications.VertexShader,
                                   VK_SHADER_STAGE_VERTEX_BIT),
            CreateShaderStageInfo_(specifications.VertexShader,
                                   VK_SHADER_STAGE_FRAGMENT_BIT),
        };

        Array<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamicStateSettings = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = static_cast<Uint32>(dynamicStates.Size()),
            .pDynamicStates = dynamicStates.Data()
        };

        VkPipelineVertexInputStateCreateInfo inputState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = Details::ToVkPrimitiveTopology_(specifications.Topology),
            .primitiveRestartEnable = VK_FALSE
        };

        VkPipelineViewportStateCreateInfo viewportState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr
        };

        VkPipelineRasterizationStateCreateInfo rasterizationState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = Details::ToVkPolygonMode_(specifications.FillMode),
            .cullMode = Details::ToVkCullMode_(specifications.CullMode),
            .frontFace = Details::ToVkFrontFace_(specifications.FrontFace),
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f
        };

        VkPipelineMultisampleStateCreateInfo multisampleState = {
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

        VkPipelineColorBlendAttachmentState colorAttachmentState = {
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

        VkPipelineColorBlendStateCreateInfo colorBlendState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &colorAttachmentState,
            .blendConstants = { 0, 0, 0, 0 }
        };

        VkFormat format = Details::ToVkFormat_(specifications.Format);
        VkPipelineRenderingCreateInfo renderingInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .pNext = nullptr,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachmentFormats = &format,
            .depthAttachmentFormat = VK_FORMAT_UNDEFINED,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED
        };

        VkGraphicsPipelineCreateInfo pipelineInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &renderingInfo,
            .flags = 0,
            .stageCount = KITSUNE_ARRAY_SIZE(shaderStages),
            .pStages = shaderStages,
            .pVertexInputState = &inputState,
            .pInputAssemblyState = &inputAssembly,
            .pTessellationState = nullptr,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizationState,
            .pMultisampleState = &multisampleState,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendState,
            .pDynamicState = &dynamicStateSettings,
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

    VulkanRenderPipeline::~VulkanRenderPipeline()
    {
        ::vkDestroyPipeline(m_Device.GetVulkanDevice(), m_Pipeline, nullptr);
        DestroyDummyLayout_();
    }

    void VulkanRenderPipeline::CreateDummyLayout_()
    {
        VkPipelineLayoutCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreatePipelineLayout(
                m_Device.GetVulkanDevice(), &createInfo, nullptr, &m_Layout),
            "Failed to create a Vulkan pipeline layout.");
    }

    void VulkanRenderPipeline::DestroyDummyLayout_()
    {
        ::vkDestroyPipelineLayout(m_Device.GetVulkanDevice(), m_Layout, nullptr);
    }

    VkPipelineShaderStageCreateInfo VulkanRenderPipeline::CreateShaderStageInfo_(
        const SharedPtr<ShaderModule>& module,
        VkShaderStageFlagBits shaderFlag)
    {
        auto vulkanModule = Details::ToImplementation_(module);
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
}
