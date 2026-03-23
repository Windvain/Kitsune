#include "RenderingCore/Vulkan/VulkanRenderingDevice.h"
#include "Foundation/Logging/GlobalLog.h"

#include "Foundation/Containers/Set.h"
#include "Foundation/Diagnostics/Assert.h"

namespace Kitsune
{
    VulkanRenderingDevice::VulkanRenderingDevice(VkPhysicalDevice physicalDevice,
                                                 const RenderingDeviceInformation& information)
        : m_Information(information)
    {
        Array<VkDeviceQueueCreateInfo> queueCreateInfos = GetDeviceQueueCreateInfo_(physicalDevice);
        VkPhysicalDeviceFeatures deviceFeatures = { /* ... */ };

        VkDeviceCreateInfo deviceCreateInfo = { /* ... */ };
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.Size();
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.Data();
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &m_Device),
            "Failed to create a Vulkan logical device.");

        ::vkGetDeviceQueue(m_Device, queueCreateInfos[0].queueFamilyIndex, 0, &m_GraphicsQueue);
        ::vkGetDeviceQueue(m_Device, queueCreateInfos[1].queueFamilyIndex, 0, &m_ComputeQueue);
        ::vkGetDeviceQueue(m_Device, queueCreateInfos[2].queueFamilyIndex, 0, &m_TransferQueue);
    }

    VulkanRenderingDevice::~VulkanRenderingDevice()
    {
        KITSUNE_ASSERT(
            m_Device != VK_NULL_HANDLE,
            "Tried to destroy a logical device which had not been initialized.");

        ::vkDestroyDevice(m_Device, nullptr);
    }

    Array<VkDeviceQueueCreateInfo> VulkanRenderingDevice::GetDeviceQueueCreateInfo_(
        VkPhysicalDevice physicalDevice)
    {
        std::uint32_t queueFamilyCount = 0;
        Array<VkQueueFamilyProperties> queueFamilyProperties;

        ::vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        queueFamilyProperties.Resize(queueFamilyCount);

        ::vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice, &queueFamilyCount, queueFamilyProperties.Data());

        Set<std::uint32_t> indices;
        const VkQueueFlags queuesToFind[3] = {
            VK_QUEUE_GRAPHICS_BIT,
            VK_QUEUE_COMPUTE_BIT,
            VK_QUEUE_TRANSFER_BIT
        };

        Array<VkDeviceQueueCreateInfo> createInfoArray;
        static const float queuePriorities[] = { 1.0f };

        for (VkQueueFlags flags : queuesToFind)
        {
            std::uint32_t currentIndex = 0;
            for (; currentIndex < queueFamilyCount; ++currentIndex)
            {
                const auto& properties = queueFamilyProperties[currentIndex];
                if (!(properties.queueFlags & flags))
                    continue;

                auto [iter_, result] = indices.Insert(currentIndex);
                if (result)
                {
                    VkDeviceQueueCreateInfo queueCreateInfo = { /* ... */ };
                    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    queueCreateInfo.queueFamilyIndex = currentIndex;
                    queueCreateInfo.queueCount = 1;
                    queueCreateInfo.pQueuePriorities = queuePriorities;

                    createInfoArray.PushBack(queueCreateInfo);
                    break;
                }
            }

            std::string flagsString = string_VkQueueFlags(
                queueFamilyProperties[currentIndex].queueFlags);

            KITSUNE_ENGINE_INFO_FORMAT_(
                "Found device queue family #{0} for {1} queue: {2}",
                currentIndex,
                string_VkQueueFlagBits(static_cast<VkQueueFlagBits>(flags)),
                flagsString.c_str());
        }

        if (createInfoArray.Size() != 3)
        {
            throw SystemException(
                "Could not find three unique queue families for the Graphics, Compute, "
                "and Transfer queues.");
        }

        return createInfoArray;
    }
}
