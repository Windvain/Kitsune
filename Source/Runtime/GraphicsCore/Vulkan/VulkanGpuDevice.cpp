#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

#include "GraphicsCore/Vulkan/VulkanFence.h"
#include "GraphicsCore/Vulkan/VulkanSemaphore.h"

#include "GraphicsCore/Vulkan/VulkanCommandList.h"
#include "GraphicsCore/Vulkan/VulkanCommandQueue.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanShaderModule.h"
#include "GraphicsCore/Vulkan/VulkanRenderPipeline.h"

namespace Kitsune
{
    namespace Details
    {
        VkPhysicalDevice GetPhysicalDeviceHandle_(const SharedPtr<GpuDevice>& device)
        {
            return DynamicPointerCast<VulkanGpuDevice>(
                device)->GetVulkanPhysicalDevice();
        }

        VkDevice GetVulkanHandle_(const SharedPtr<GpuDevice>& device)
        {
            return DynamicPointerCast<VulkanGpuDevice>(device)->GetVulkanDevice();
        }
    }

    VulkanGpuDevice::VulkanGpuDevice(
        VkPhysicalDevice physicalDevice,
        const Array<VkDeviceQueueCreateInfo>& queueInfos,
        const Array<const char*>& extensions)
        : m_PhysicalDevice(physicalDevice)
    {
        VkPhysicalDeviceFeatures features = { /* ... */ };

        // Wireframe rendering is only used when debugging.
#if !defined(KITSUNE_BUILD_PRODUCTION)
        features.fillModeNonSolid = VK_TRUE;
#endif

        VkPhysicalDeviceVulkan13Features vulkan13Features = { /* ... */ };
        vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vulkan13Features.pNext = nullptr;
        vulkan13Features.synchronization2 = VK_TRUE;    // For vkCmdPipelineBarrier2.
        vulkan13Features.dynamicRendering = VK_TRUE;

        VkDeviceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &vulkan13Features,
            .flags = 0,
            .queueCreateInfoCount = static_cast<Uint32>(queueInfos.Size()),
            .pQueueCreateInfos = queueInfos.Data(),
            .enabledLayerCount = 0,             // Deprecated.
            .ppEnabledLayerNames = nullptr,     // Deprecated.
            .enabledExtensionCount = static_cast<Uint32>(extensions.Size()),
            .ppEnabledExtensionNames = extensions.Data(),
            .pEnabledFeatures = &features
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_Device),
            "Failed to create a Vulkan logical device.");

        m_CommandQueues.Resize(queueInfos.Size());
        for (Uint32 index = 0; index < queueInfos.Size(); ++index)
        {
            const VkDeviceQueueCreateInfo& queueInfo = queueInfos[index];
            for (Uint32 queueIndex = 0; queueIndex < queueInfo.queueCount; ++queueIndex)
            {
                m_CommandQueues[index].PushBack(
                    MakeShared<VulkanCommandQueue>(
                        *this,
                        queueInfo.queueFamilyIndex,
                        queueIndex));
            }
        }
    }

    VulkanGpuDevice::~VulkanGpuDevice()
    {
        ::vkDestroyDevice(m_Device, nullptr);
    }

    SharedPtr<CommandQueue> VulkanGpuDevice::GetQueue(
        Uint32 index, Uint32 queueIndex) const
    {
        return m_CommandQueues[index][queueIndex];
    }

    SharedPtr<Fence> VulkanGpuDevice::CreateFence()
    {
        return MakeShared<VulkanFence>(*this);
    }

    SharedPtr<Semaphore> VulkanGpuDevice::MakeSemaphore()
    {
        return MakeShared<VulkanSemaphore>(*this);
    }

    SharedPtr<CommandPool> VulkanGpuDevice::CreateCommandPool(
        const SharedPtr<CommandQueue>& queue)
    {
        return MakeShared<VulkanCommandPool>(
            *this, DynamicPointerCast<VulkanCommandQueue>(queue));
    }

    SharedPtr<RenderPipeline> VulkanGpuDevice::CreateRenderPipeline(
        const RenderPipelineSpecifications& specifications)
    {
        return MakeShared<VulkanRenderPipeline>(*this, specifications);
    }

    SharedPtr<ShaderModule> VulkanGpuDevice::CreateShaderModule(
        const Byte* shaderSource, Usize sourceSize)
    {
        return MakeShared<VulkanShaderModule>(*this, shaderSource, sourceSize);
    }

    SharedPtr<TextureView> VulkanGpuDevice::CreateTextureView(
        const SharedPtr<Texture>& texture,
        const TextureViewSpecifications& specifications)
    {
        return MakeShared<VulkanTextureView>(
            *this,
            DynamicPointerCast<VulkanTexture>(texture),
            specifications);
    }
}
