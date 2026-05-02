#pragma once

#include "Foundation/Memory/SharedPtr.h"

#include "GraphicsCore/CommandQueue.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanGpuDevice;
    class VulkanCommandQueue;

    namespace Details
    {
        [[nodiscard]]
        VkQueueFlags ToVkQueueFlags_(CommandQueueType queueType);

        [[nodiscard]]
        SharedPtr<VulkanCommandQueue> ToImplementation_(
            const SharedPtr<CommandQueue>& commandQueue);
    }

    class VulkanCommandQueue : public CommandQueue
    {
    public:
        VulkanCommandQueue(VulkanGpuDevice& device, Uint32 familyIndex,
                           Uint32 queueIndex);

    public:
        void Submit(
            const Array<SharedPtr<CommandList>>& commandLists,
            const CommandQueueSubmitInformation& information,
            const SharedPtr<Fence>& fence) override;

        void WaitIdle() override;

    public:
        [[nodiscard]]
        inline VkQueue GetVulkanQueue() const
        {
            return m_Queue;
        }

        [[nodiscard]]
        inline Uint32 GetFamilyIndex() const
        {
            return m_FamilyIndex;
        }

        [[nodiscard]]
        inline Uint32 GetQueueIndex() const
        {
            return m_QueueIndex;
        }

    private:
        VulkanGpuDevice& m_Device;
        VkQueue m_Queue = VK_NULL_HANDLE;

        Uint32 m_FamilyIndex;
        Uint32 m_QueueIndex;
    };
}
