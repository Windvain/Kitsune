#include "GraphicsCore/Vulkan/VulkanCommandList.h"

#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanCommandQueue.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanRenderPipeline.h"

#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    namespace Details
    {
        VkCommandBuffer GetVulkanHandle_(const SharedPtr<CommandList>& commandList)
        {
            return DynamicPointerCast<VulkanCommandList>(
                commandList)->GetVulkanCommandBuffer();
        }
    }

    VulkanCommandPool::VulkanCommandPool(VulkanGpuDevice& device,
                                         const SharedPtr<VulkanCommandQueue>& queue)
        : m_Device(device)
    {
        if (queue == nullptr)
        {
            throw InvalidArgumentException(
                "Could not create a Vulkan command pool. The queue handle passed in to "
                "VulkanCommandPool() is a nullptr.");
        }

        VkCommandPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue->GetFamilyIndex()
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateCommandPool(m_Device.GetVulkanDevice(), &createInfo, nullptr,
                                  &m_CommandPool),
            "Failed to create a Vulkan command pool.");
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        ::vkDestroyCommandPool(m_Device.GetVulkanDevice(), m_CommandPool, nullptr);
    }

    SharedPtr<CommandList> VulkanCommandPool::AllocateCommandList()
    {
        return MakeShared<VulkanCommandList>(m_Device, *this);
    }

    VulkanCommandList::VulkanCommandList(VulkanGpuDevice& device,
                                         VulkanCommandPool& commandPool)
        : m_Device(device), m_CommandPool(commandPool)
    {
        VkCommandBufferAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = m_CommandPool.GetVulkanPool(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkAllocateCommandBuffers(m_Device.GetVulkanDevice(), &allocInfo,
                                       &m_CommandBuffer),
            "Failed to allocate a command buffer from the Vulkan command pool.");
    }

    VulkanCommandList::~VulkanCommandList()
    {
        ::vkFreeCommandBuffers(
            m_Device.GetVulkanDevice(), m_CommandPool.GetVulkanPool(),
            1, &m_CommandBuffer);
    }

    void VulkanCommandList::Begin()
    {
        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkBeginCommandBuffer(m_CommandBuffer, &beginInfo),
            "Failed to begin Vulkan command buffer recording.");
    }

    void VulkanCommandList::End()
    {
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEndCommandBuffer(m_CommandBuffer),
            "Failed to end Vulkan command buffer recording.");
    }

    void VulkanCommandList::BeginRendering(
        const SharedPtr<TextureView>& texture,
        const RenderingSpecifications& specifications)
    {
        if (texture == nullptr)
        {
            throw InvalidArgumentException(
                "Failed to begin a render pass. The given texture meant as a render "
                "target is invalid. (NULL)");
        }

        VkImageView imageView = Details::GetVulkanHandle_(texture);
        VkClearColorValue clearColorValue = {
            .float32 = { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        VkRenderingAttachmentInfo renderingAttachmentInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = imageView,
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = { clearColorValue }
        };

        VkRenderingInfo renderingInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderArea = Details::EngineToVulkan_(specifications.RenderArea),
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachments = &renderingAttachmentInfo,
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr
        };

        ::vkCmdBeginRendering(m_CommandBuffer, &renderingInfo);
    }

    void VulkanCommandList::EndRendering()
    {
        ::vkCmdEndRendering(m_CommandBuffer);
    }

    void VulkanCommandList::SetViewport(
        const Rect2<float>& viewport,
        float minimumDepth, float maximumDepth)
    {
        VkViewport vulkanViewport = {
            .x = viewport.Position.X,
            .y = viewport.Position.Y,
            .width = viewport.Size.X,
            .height = viewport.Size.Y,

            .minDepth = minimumDepth,
            .maxDepth = maximumDepth
        };

        ::vkCmdSetViewport(m_CommandBuffer, 0, 1, &vulkanViewport);
    }

    void VulkanCommandList::SetScissor(const Rect2<Uint32>& scissorRect)
    {
        VkRect2D vulkanScissor = Details::EngineToVulkan_(scissorRect);
        ::vkCmdSetScissor(m_CommandBuffer, 0, 1, &vulkanScissor);
    }

    void VulkanCommandList::BindRenderPipeline(
        const SharedPtr<RenderPipeline>& pipeline)
    {
        ::vkCmdBindPipeline(
            m_CommandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            Details::GetVulkanHandle_(pipeline));
    }

    void VulkanCommandList::Draw(Uint32 vertexCount, Uint32 instanceCount,
                                 Uint32 firstVertex, Uint32 firstInstance)
    {
        ::vkCmdDraw(m_CommandBuffer, vertexCount, instanceCount,
                    firstVertex, firstInstance);
    }

    void VulkanCommandList::Reset()
    {
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkResetCommandBuffer(m_CommandBuffer, 0),
            "Failed to reset a Vulkan command buffer!");
    }

    void VulkanCommandList::TextureBarrier(
        const SharedPtr<Texture>& texture,
        const TextureBarrierDescription& description)
    {
        VkImageMemoryBarrier2 barrier = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = GetPipelineStage_(description.PreviousUsage),
            .srcAccessMask = GetAccessFlags_(description.PreviousUsage),
            .dstStageMask = GetPipelineStage_(description.NewUsage),
            .dstAccessMask = GetAccessFlags_(description.NewUsage),
            .oldLayout = Details::EngineToVulkan_(description.PreviousUsage),
            .newLayout = Details::EngineToVulkan_(description.NewUsage),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = Details::GetVulkanHandle_(texture),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VkDependencyInfo dependencyInfo = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .pNext = nullptr,
            .dependencyFlags = 0,
            .memoryBarrierCount = 0,
            .pMemoryBarriers = nullptr,
            .bufferMemoryBarrierCount = 0,
            .pBufferMemoryBarriers = nullptr,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier
        };

        ::vkCmdPipelineBarrier2(m_CommandBuffer, &dependencyInfo);
    }

    VkAccessFlags2 VulkanCommandList::GetAccessFlags_(TextureUsage textureUsage)
    {
        switch (textureUsage)
        {
        case TextureUsage::Undefined:
        case TextureUsage::Presentation:
            return { /* ... */ };
        case TextureUsage::RenderAttachment:
            return VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        }

        KITSUNE_UNREACHABLE();
    }

    VkPipelineStageFlags2 VulkanCommandList::GetPipelineStage_(
        TextureUsage textureUsage)
    {
        switch (textureUsage)
        {
        case TextureUsage::Undefined:
        case TextureUsage::RenderAttachment:
            return VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        case Kitsune::TextureUsage::Presentation:
            return VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
        }

        KITSUNE_UNREACHABLE();
    }
}
