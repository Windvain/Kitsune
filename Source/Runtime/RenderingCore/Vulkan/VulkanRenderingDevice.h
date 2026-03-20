#pragma once

#include "RenderingCore/RenderingDevice.h"
#include "RenderingCore/Vulkan/VulkanHeader.h"

#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    class VulkanRenderingDevice : public RenderingDevice
    {
    public:
        VulkanRenderingDevice(VkPhysicalDevice physicalDevice,
                              const RenderingDeviceInformation& information);

        ~VulkanRenderingDevice();

    public:
        inline RenderingDeviceInformation GetInformation() const override
        {
            return m_Information;
        }

    private:
        static Array<VkDeviceQueueCreateInfo> GetDeviceQueueCreateInfo_(
            VkPhysicalDevice physicalDevice);

    private:
        VkDevice m_Device;
        RenderingDeviceInformation m_Information;
    };
}
