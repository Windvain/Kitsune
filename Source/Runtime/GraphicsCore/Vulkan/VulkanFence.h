#pragma once

#include "GraphicsCore/Fence.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

#include "Foundation/Memory/SharedPtr.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]] VkFence GetVulkanHandle_(const SharedPtr<Fence>& fence);
    }

    class VulkanGpuDevice;

    class VulkanFence : public Fence
    {
    public:
        VulkanFence(VulkanGpuDevice& device);
        ~VulkanFence() override;

    public:
        void Wait(Uint64 timeout) override;
        void Reset() override;

    public:
        [[nodiscard]]
        inline VkFence GetVulkanFence() const
        {
            return m_Fence;
        }

    private:
        VulkanGpuDevice& m_Device;
        VkFence m_Fence = VK_NULL_HANDLE;
    };
}
