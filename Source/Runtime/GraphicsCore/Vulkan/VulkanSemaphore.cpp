#include "GraphicsCore/Vulkan/VulkanSemaphore.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

namespace Kitsune
{
    namespace Details
    {
        SharedPtr<VulkanSemaphore> ToImplementation_(
            const SharedPtr<Semaphore>& semaphore)
        {
            return DynamicPointerCast<VulkanSemaphore>(semaphore);
        }
    }

    VulkanSemaphore::VulkanSemaphore(VulkanGpuDevice& device)
        : m_Device(device)
    {
        VkSemaphoreCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateSemaphore(m_Device.GetVulkanDevice(), &createInfo, nullptr,
                                &m_Semaphore),
            "Failed to create a Vulkan semaphore!");
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        ::vkDestroySemaphore(m_Device.GetVulkanDevice(), m_Semaphore, nullptr);
    }
}
