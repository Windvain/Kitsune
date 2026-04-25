#pragma once

#include "GraphicsCore/CommandQueue.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]] VkQueueFlags EngineToVulkan_(CommandQueueType queueType);
    }

    class VulkanGpuDevice;

    class VulkanCommandQueue : public CommandQueue
    {
    public:
        VulkanCommandQueue(VulkanGpuDevice& device,
                           Uint32 familyIndex, Uint32 queueIndex);

    public:
        void Submit(
            const Array<SharedPtr<CommandList>>& commandLists,
            const SharedPtr<Semaphore>& waitSemaphore,
            SharedPtr<Semaphore>& signaledSemaphore,
            SharedPtr<Fence>& signaledFence) override;

        void Present(const SharedPtr<RenderSurface>& surface,
                     Uint32 backBufferIndex,
                     const SharedPtr<Semaphore>& waitSemaphore) override;

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
