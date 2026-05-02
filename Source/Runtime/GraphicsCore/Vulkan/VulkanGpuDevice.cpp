#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

#include "GraphicsCore/Vulkan/VulkanFence.h"
#include "GraphicsCore/Vulkan/VulkanSemaphore.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanSwapChain.h"
#include "GraphicsCore/Vulkan/VulkanCommandList.h"
#include "GraphicsCore/Vulkan/VulkanCommandQueue.h"

#include "GraphicsCore/Vulkan/VulkanShaderModule.h"
#include "GraphicsCore/Vulkan/VulkanRenderSurface.h"
#include "GraphicsCore/Vulkan/VulkanRenderPipeline.h"

namespace Kitsune
{
    VulkanGpuDevice::VulkanGpuDevice(
        VkPhysicalDevice physicalDevice,
        const Array<VkDeviceQueueCreateInfo>& queueInfos,
        const Array<const char*>& extensions,
        GpuDeviceFeature features)
        : m_PhysicalDevice(physicalDevice), m_Features(features)
    {
        VkPhysicalDeviceFeatures deviceFeatures = { /* ... */ };

        if (bool(features & GpuDeviceFeature::WireframeRendering))
            deviceFeatures.fillModeNonSolid = VK_TRUE;

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
            .pEnabledFeatures = &deviceFeatures
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

    SharedPtr<CommandQueue> VulkanGpuDevice::GetCommandQueue(
        Uint32 index, Uint32 queueIndex) const
    {
        return m_CommandQueues[index][queueIndex];
    }

    SharedPtr<CommandPool> VulkanGpuDevice::CreateCommandPool(
        const SharedPtr<CommandQueue>& commandQueue)
    {
        return MakeShared<VulkanCommandPool>(
            *this,
            Details::ToImplementation_(commandQueue));
    }

    SharedPtr<Fence> VulkanGpuDevice::CreateFence(FenceFlag flags)
    {
        return MakeShared<VulkanFence>(*this, flags);
    }

    SharedPtr<RenderPipeline> VulkanGpuDevice::CreateRenderPipeline(
        const RenderPipelineSpecifications& specifications)
    {
        return MakeShared<VulkanRenderPipeline>(*this, specifications);
    }

    SharedPtr<Semaphore> VulkanGpuDevice::MakeSemaphore()
    {
        return MakeShared<VulkanSemaphore>(*this);
    }

    SharedPtr<SwapChain> VulkanGpuDevice::CreateSwapChain(
        const SharedPtr<RenderSurface>& surface,
        const SharedPtr<CommandQueue>& presentQueue,
        const SwapChainConfiguration& configuration)
    {
        return MakeShared<VulkanSwapChain>(
            *this,
            Details::ToImplementation_(surface),
            Details::ToImplementation_(presentQueue),
            configuration);
    }

    SharedPtr<TextureView> VulkanGpuDevice::CreateTextureView(
        const SharedPtr<Texture>& texture,
        const TextureViewSpecifications& specifications)
    {
        auto vulkanTexture = Details::ToImplementation_(texture);
        return MakeShared<VulkanTextureView>(
            *this,
            vulkanTexture,
            specifications);
    }

    SharedPtr<ShaderModule> VulkanGpuDevice::CreateShaderModule(Array<Byte>&& shaderCode)
    {
        return MakeShared<VulkanShaderModule>(*this, Move(shaderCode));
    }
}
