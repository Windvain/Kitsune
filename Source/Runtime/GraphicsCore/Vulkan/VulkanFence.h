#pragma once

#include "Foundation/Memory/SharedPtr.h"

#include "GraphicsCore/Fence.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanFence;
    class VulkanGpuDevice;

    namespace Details
    {
        [[nodiscard]] VkFenceCreateFlags ToVkFenceCreateFlags_(FenceFlag flags);

        [[nodiscard]]
        SharedPtr<VulkanFence> ToImplementation_(const SharedPtr<Fence>& fence);
    }

    class VulkanFence : public Fence
    {
    public:
        VulkanFence(VulkanGpuDevice& device, FenceFlag flags);
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
