#include "GraphicsCore/Vulkan/VulkanShaderModule.h"
#include "GraphicsCore/Vulkan//VulkanGpuDevice.h"

namespace Kitsune
{
    namespace Details
    {
        SharedPtr<VulkanShaderModule> ToImplementation_(
            const SharedPtr<ShaderModule>& shaderModule)
        {
            return DynamicPointerCast<VulkanShaderModule>(shaderModule);
        }
    }

    VulkanShaderModule::VulkanShaderModule(VulkanGpuDevice& device,
                                           Array<Byte>&& shaderSource)
        : m_Device(device), m_ShaderSource(Move(shaderSource))
    {
        VkShaderModuleCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = m_ShaderSource.Size(),
            .pCode = reinterpret_cast<const std::uint32_t*>(m_ShaderSource.Data())
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
