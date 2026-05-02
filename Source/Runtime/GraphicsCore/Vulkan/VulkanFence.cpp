#include "GraphicsCore/Vulkan/VulkanFence.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

namespace Kitsune
{
    namespace Details
    {
        VkFenceCreateFlags ToVkFenceCreateFlags_(FenceFlag flags)
        {
            return bool(flags & FenceFlag::Signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        }

        SharedPtr<VulkanFence> ToImplementation_(const SharedPtr<Fence>& fence)
        {
            return DynamicPointerCast<VulkanFence>(fence);
        }
    }

    VulkanFence::VulkanFence(VulkanGpuDevice& device, FenceFlag flags)
        : m_Device(device)
    {
        VkFenceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = Details::ToVkFenceCreateFlags_(flags)
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateFence(m_Device.GetVulkanDevice(), &createInfo, nullptr, &m_Fence),
            "Failed to create a Vulkan fence!");
    }

    VulkanFence::~VulkanFence()
    {
        ::vkDestroyFence(m_Device.GetVulkanDevice(), m_Fence, nullptr);
    }

    void VulkanFence::Wait(Uint64 timeout)
    {
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkWaitForFences(m_Device.GetVulkanDevice(), 1, &m_Fence, VK_TRUE, timeout),
            "Failed to wait for a Vulkan fence.");
    }

    void VulkanFence::Reset()
    {
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkResetFences(m_Device.GetVulkanDevice(), 1, &m_Fence),
            "Failed to reset a Vulkan fence!");
    }
}
