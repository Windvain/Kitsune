#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "GraphicsCore/ShaderModule.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanGpuDevice;
    class VulkanShaderModule;

    namespace Details
    {
        SharedPtr<VulkanShaderModule> ToImplementation_(
            const SharedPtr<ShaderModule>& shaderModule);
    }

    class VulkanShaderModule : public ShaderModule
    {
    public:
        VulkanShaderModule(VulkanGpuDevice& device, Array<Byte>&& shaderSource);
        ~VulkanShaderModule() override;

    public:
        [[nodiscard]]
        inline VkShaderModule GetVulkanShaderModule() const
        {
            return m_ShaderModule;
        }

    private:
        VulkanGpuDevice& m_Device;

        Array<Byte> m_ShaderSource;
        VkShaderModule m_ShaderModule = VK_NULL_HANDLE;
    };
}
