#include "GraphicsCore/Vulkan/VulkanGpuBuffer.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

namespace Kitsune
{
    namespace Details
    {
        VkBufferUsageFlags ToVkBufferUsage_(GpuBufferUsage usage)
        {
            VkBufferUsageFlags flags = 0;
            if (bool(usage & GpuBufferUsage::VertexBuffer))
                flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            if (bool(usage & GpuBufferUsage::IndexBuffer))
                flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            if (bool(usage & GpuBufferUsage::TransferSource))
                flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            if (bool(usage & GpuBufferUsage::TransferDestination))
                flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

            return flags;
        }

        VkMemoryPropertyFlags ToVkMemoryProperties_(GpuBufferUsage usage)
        {
            VkMemoryPropertyFlags flags = 0;
            if (bool(usage & GpuBufferUsage::TransferSource))
            {
                flags |= (VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            }
            else
            {
                flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            }

            return flags;
        }

        SharedPtr<VulkanGpuBuffer> ToImplementation_(const SharedPtr<GpuBuffer>& buffer)
        {
            return DynamicPointerCast<VulkanGpuBuffer>(buffer);
        }
    }

    VulkanGpuBuffer::VulkanGpuBuffer(
        VulkanGpuDevice& device,
        const GpuBufferSpecifications& specifications)
        : m_Device(device)
    {
        VkBufferCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = specifications.Size,
            .usage = Details::ToVkBufferUsage_(specifications.Usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateBuffer(m_Device.GetVulkanDevice(), &createInfo, nullptr,
                             &m_Buffer),
            "Failed to create a Vulkan buffer.");

        VkMemoryRequirements memoryRequirements;
        ::vkGetBufferMemoryRequirements(
            m_Device.GetVulkanDevice(),
            m_Buffer,
            &memoryRequirements);

        m_DeviceMemory = m_Device.AllocateMemory(
            memoryRequirements.size,
            memoryRequirements.memoryTypeBits,
            Details::ToVkMemoryProperties_(specifications.Usage));

        ::vkBindBufferMemory(m_Device.GetVulkanDevice(), m_Buffer, m_DeviceMemory, 0);
    }

    VulkanGpuBuffer::~VulkanGpuBuffer()
    {
        m_Device.FreeMemory(m_DeviceMemory);
        ::vkDestroyBuffer(m_Device.GetVulkanDevice(), m_Buffer, nullptr);
    }

    void* VulkanGpuBuffer::Map()
    {
        void* cpuMemory;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkMapMemory(
                m_Device.GetVulkanDevice(), m_DeviceMemory, 0, VK_WHOLE_SIZE,
                0, &cpuMemory),
            "Failed to map GPU memory to the application's address space.");

        return cpuMemory;
    }

    void VulkanGpuBuffer::Unmap()
    {
        ::vkUnmapMemory(m_Device.GetVulkanDevice(), m_DeviceMemory);
    }
}
