#include "RenderingCore/Vulkan/VulkanRenderingDevice.h"

#include "Foundation/Logging/GlobalLog.h"
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
        ::vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        Array<VkQueueFamilyProperties> properties(queueFamilyCount, VkQueueFamilyProperties());
        ::vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice, &queueFamilyCount, properties.Data());

        Array<VkDeviceQueueCreateInfo> createInfoArray;
        std::uint32_t queueFamilyIndex = 0;

        KITSUNE_ENGINE_INFO_("Retrieving all usable queues from physical device:");
        for (const VkQueueFamilyProperties& queueProperty : properties)
        {
            const float queuePriority = 1.0f;
            const VkQueueFlags requestedFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT |
                                                VK_QUEUE_TRANSFER_BIT;

            if ((queueProperty.queueFlags & requestedFlags) == 0)
                continue;

            std::string queueFlagsStr = string_VkQueueFlags(queueProperty.queueFlags);

            KITSUNE_UNUSED(queueFlagsStr);
            KITSUNE_ENGINE_INFO_FORMAT_(
                "\tQueue #{0}: {1}",
                queueFamilyIndex, queueFlagsStr.c_str());

            VkDeviceQueueCreateInfo createInfo = { /* ... */ };
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.queueFamilyIndex = queueFamilyIndex;
            createInfo.queueCount = 1;
            createInfo.pQueuePriorities = &queuePriority;

            createInfoArray.PushBack(createInfo);
            ++queueFamilyIndex;
        }

        if (createInfoArray.IsEmpty())
            throw SystemException("Could not find any suitable Vulkan queues.");

        KITSUNE_ENGINE_INFO_FORMAT_("Found {0} suitable queues.", createInfoArray.Size());
        return createInfoArray;
    }
}
