#include "GraphicsCore/Vulkan/VulkanCommandQueue.h"

#include "GraphicsCore/Vulkan/VulkanFence.h"
#include "GraphicsCore/Vulkan/VulkanSemaphore.h"

#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanCommandList.h"

namespace Kitsune
{
    namespace Details
    {
        VkQueueFlags ToVkQueueFlags_(CommandQueueType queueType)
        {
            switch (queueType)
            {
            case CommandQueueType::Graphics:
                return VK_QUEUE_GRAPHICS_BIT;
            case CommandQueueType::Compute:
                return VK_QUEUE_COMPUTE_BIT;
            case CommandQueueType::Transfer:
                return VK_QUEUE_TRANSFER_BIT;
            }

            KITSUNE_UNREACHABLE();
        }

        SharedPtr<VulkanCommandQueue> ToImplementation_(
            const SharedPtr<CommandQueue>& commandQueue)
        {
            return DynamicPointerCast<VulkanCommandQueue>(commandQueue);
        }
    }

    VulkanCommandQueue::VulkanCommandQueue(VulkanGpuDevice& device, Uint32 familyIndex,
                                           Uint32 queueIndex)
        : m_Device(device),
          m_FamilyIndex(familyIndex),
          m_QueueIndex(queueIndex)
    {
        ::vkGetDeviceQueue(
            m_Device.GetVulkanDevice(), familyIndex, queueIndex, &m_Queue);
    }

    void VulkanCommandQueue::Submit(
        const Array<SharedPtr<CommandList>>& commandLists,
        const CommandQueueSubmitInformation& information,
        const SharedPtr<Fence>& fence)
    {
        VkPipelineStageFlags waitDestStage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        Array<VkSemaphore> waited;
        Array<VkSemaphore> signaled;

        for (const SharedPtr<Semaphore>& semaphore : information.Waited)
            waited.PushBack(Details::ToImplementation_(semaphore)->GetVulkanSemaphore());

        for (const SharedPtr<Semaphore>& semaphore : information.Signaled)
        {
            signaled.PushBack(
                Details::ToImplementation_(semaphore)->GetVulkanSemaphore());
        }

        Array<VkCommandBuffer> commandBuffers;
        for (const SharedPtr<CommandList>& commandList : commandLists)
        {
            auto vulkanCommandList = Details::ToImplementation_(commandList);
            commandBuffers.PushBack(vulkanCommandList->GetVulkanCommandBuffer());
        }

        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = static_cast<Uint32>(waited.Size()),
            .pWaitSemaphores = waited.Data(),
            .pWaitDstStageMask = &waitDestStage,
            .commandBufferCount = static_cast<Uint32>(commandLists.Size()),
            .pCommandBuffers = commandBuffers.Data(),
            .signalSemaphoreCount = static_cast<Uint32>(signaled.Size()),
            .pSignalSemaphores = signaled.Data()
        };

        VkFence vulkanFence = (fence != nullptr) ?
            Details::ToImplementation_(fence)->GetVulkanFence() :
            VK_NULL_HANDLE;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkQueueSubmit(m_Queue, 1, &submitInfo, vulkanFence),
            "Failed to submit Vulkan command buffers to a queue.");
    }

    void VulkanCommandQueue::WaitIdle()
    {
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkQueueWaitIdle(m_Queue),
            "Failed to wait for the queue to be idle.");
    }
}
