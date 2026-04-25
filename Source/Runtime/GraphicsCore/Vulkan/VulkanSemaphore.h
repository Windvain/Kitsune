#pragma once

#include "GraphicsCore/Semaphore.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

#include "Foundation/Memory/SharedPtr.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]]
        VkSemaphore GetVulkanHandle_(const SharedPtr<Semaphore>& semaphore);
    }

    class VulkanGpuDevice;

    class VulkanSemaphore : public Semaphore
    {
    public:
        VulkanSemaphore(VulkanGpuDevice& device);
        ~VulkanSemaphore() override;

    public:
        [[nodiscard]]
        inline VkSemaphore GetVulkanSemaphore() const
        {
            return m_Semaphore;
        }

    private:
        VulkanGpuDevice& m_Device;
        VkSemaphore m_Semaphore = VK_NULL_HANDLE;
    };
}
