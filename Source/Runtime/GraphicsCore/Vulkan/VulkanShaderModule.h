#pragma once

#include "GraphicsCore/ShaderModule.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanGpuDevice;

    class VulkanShaderModule : public ShaderModule
    {
    public:
        VulkanShaderModule(
            VulkanGpuDevice& device,
            const Byte* shaderSource, Usize sourceSize);

        ~VulkanShaderModule() override;

    public:
        [[nodiscard]]
        inline VkShaderModule GetVulkanShaderModule() const
        {
            return m_ShaderModule;
        }

    private:
        VulkanGpuDevice& m_Device;
        VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
    };
}
