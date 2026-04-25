#include "GraphicsCore/Vulkan/VulkanCommandQueue.h"

#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanRenderSurface.h"

#include "GraphicsCore/Vulkan/VulkanFence.h"
#include "GraphicsCore/Vulkan/VulkanSemaphore.h"
#include "GraphicsCore/Vulkan/VulkanCommandList.h"

#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    namespace Details
    {
        VkQueueFlags EngineToVulkan_(CommandQueueType queueType)
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
    }

    VulkanCommandQueue::VulkanCommandQueue(VulkanGpuDevice& device,
                                           Uint32 familyIndex, Uint32 queueIndex)
        : m_Device(device),
          m_FamilyIndex(familyIndex),
          m_QueueIndex(queueIndex)
    {
        ::vkGetDeviceQueue(
            m_Device.GetVulkanDevice(), familyIndex, queueIndex, &m_Queue);
    }

    void VulkanCommandQueue::Submit(
        const Array<SharedPtr<CommandList>>& commandLists,
        const SharedPtr<Semaphore>& waitSemaphore,
        SharedPtr<Semaphore>& signaledSemaphore,
        SharedPtr<Fence>& signaledFence)
    {
        Array<VkCommandBuffer> commandBuffers;
        for (const SharedPtr<CommandList>& commandList : commandLists)
            commandBuffers.PushBack(Details::GetVulkanHandle_(commandList));

        VkSemaphore vkWaitSemaphore = Details::GetVulkanHandle_(waitSemaphore);
        VkSemaphore vkSignaledSemaphore = Details::GetVulkanHandle_(signaledSemaphore);
        VkFence vkSignaledFence = Details::GetVulkanHandle_(signaledFence);

        VkPipelineStageFlags waitDestStage =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = (vkWaitSemaphore != VK_NULL_HANDLE),
            .pWaitSemaphores = &vkWaitSemaphore,
            .pWaitDstStageMask = &waitDestStage,
            .commandBufferCount = static_cast<Uint32>(commandBuffers.Size()),
            .pCommandBuffers = commandBuffers.Data(),
            .signalSemaphoreCount = (vkSignaledSemaphore != VK_NULL_HANDLE),
            .pSignalSemaphores = &vkSignaledSemaphore
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkQueueSubmit(m_Queue, commandBuffers.Size(), &submitInfo,
                            vkSignaledFence),
            "Failed to submit Vulkan command buffers to a queue.");
    }

    void VulkanCommandQueue::Present(const SharedPtr<RenderSurface>& surface,
                                     Uint32 backBufferIndex,
                                     const SharedPtr<Semaphore>& waitSemaphore)
    {
        if (surface == nullptr)
        {
            throw InvalidArgumentException(
                "Failed to submit a present command to the queue. The surface given "
                "is invalid. (NULL)");
        }

        VkSemaphore vulkanSemaphore = Details::GetVulkanHandle_(waitSemaphore);
        VkSwapchainKHR swapChain = DynamicPointerCast<VulkanRenderSurface>(
            surface)->GetVulkanSwapChain();

        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &vulkanSemaphore,
            .swapchainCount = (vulkanSemaphore != VK_NULL_HANDLE),
            .pSwapchains = &swapChain,
            .pImageIndices = &backBufferIndex,
            .pResults = nullptr
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkQueuePresentKHR(m_Queue, &presentInfo),
            "Failed to present command buffers.");
    }
}
