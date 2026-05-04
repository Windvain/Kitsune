#pragma once

#include "Foundation/Memory/SharedPtr.h"

#include "GraphicsCore/GpuBuffer.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanGpuDevice;
    class VulkanGpuBuffer;

    namespace Details
    {
        [[nodiscard]]
        VkBufferUsageFlags ToVkBufferUsage_(GpuBufferUsage usage);

        [[nodiscard]]
        VkMemoryPropertyFlags ToVkMemoryProperties_(GpuBufferUsage usage);

        [[nodiscard]]
        SharedPtr<VulkanGpuBuffer> ToImplementation_(const SharedPtr<GpuBuffer>& buffer);
    }

    class VulkanGpuBuffer : public GpuBuffer
    {
    public:
        VulkanGpuBuffer(
            VulkanGpuDevice& device,
            const GpuBufferSpecifications& specifications);

        ~VulkanGpuBuffer() override;

    public:
        [[nodiscard]] void* Map() override;
        void Unmap() override;

    public:
        [[nodiscard]]
        inline VkBuffer GetVulkanBuffer() const
        {
            return m_Buffer;
        }

    private:
        VulkanGpuDevice& m_Device;

        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_DeviceMemory;
    };
}
