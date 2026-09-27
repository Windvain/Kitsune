#include "Graphics/Vulkan/VulkanGraphicsDevice.h"

namespace Kitsune
{
    VulkanGraphicsDevice::VulkanGraphicsDevice(VkPhysicalDevice physicalDevice,
                                               GraphicsDeviceDescription&& description)
        : m_PhysicalDevice(physicalDevice), m_Description(Move(description))
    {
        KITSUNE_UNUSED(m_PhysicalDevice);
    }
}
