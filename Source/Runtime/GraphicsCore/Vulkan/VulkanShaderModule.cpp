#include "GraphicsCore/Vulkan/VulkanShaderModule.h"
#include "GraphicsCore/Vulkan//VulkanGpuDevice.h"

namespace Kitsune
{
    VulkanShaderModule::VulkanShaderModule(
        VulkanGpuDevice& device,
        const Byte* shaderSource, Usize sourceSize)
        : m_Device(device)
    {
        VkShaderModuleCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = sourceSize,
            .pCode = reinterpret_cast<const std::uint32_t*>(shaderSource)
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateShaderModule(
                m_Device.GetVulkanDevice(), &createInfo, nullptr, &m_ShaderModule),
            "Failed to create a Vulkan shader module.");
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        ::vkDestroyShaderModule(m_Device.GetVulkanDevice(), m_ShaderModule, nullptr);
    }
}
