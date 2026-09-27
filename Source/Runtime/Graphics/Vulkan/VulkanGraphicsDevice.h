#pragma once

#include "Graphics/GraphicsDevice.h"
#include "Graphics/Vulkan/VulkanHeader.h"       // IWYU pragma: keep

namespace Kitsune
{
    class VulkanGraphicsDevice : public GraphicsDevice
    {
    public:
        KITSUNE_API VulkanGraphicsDevice(
            VkPhysicalDevice physicalDevice,
            GraphicsDeviceDescription&& description);

    public:
        [[nodiscard]]
        inline GraphicsDeviceDescription GetDescription() const override
        {
            return m_Description;
        }

    private:
        VkPhysicalDevice m_PhysicalDevice;
        GraphicsDeviceDescription m_Description;
    };
}
