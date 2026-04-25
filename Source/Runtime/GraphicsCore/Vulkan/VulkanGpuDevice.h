#pragma once

#include "GraphicsCore/GpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]]
        VkPhysicalDevice GetPhysicalDeviceHandle_(const SharedPtr<GpuDevice>& device);

        [[nodiscard]]
        VkDevice GetVulkanHandle_(const SharedPtr<GpuDevice>& device);
    }

    class VulkanCommandQueue;

    class VulkanGpuDevice : public GpuDevice
    {
    public:
        VulkanGpuDevice(
            VkPhysicalDevice physicalDevice,
            const Array<VkDeviceQueueCreateInfo>& queueInfos,
            const Array<const char*>& extensions);

        ~VulkanGpuDevice() override;

    public:
        [[nodiscard]]
        SharedPtr<CommandQueue> GetQueue(
            Uint32 index, Uint32 queueIndex) const override;

    public:
        [[nodiscard]]
        SharedPtr<Fence> CreateFence() override;

        [[nodiscard]]
        SharedPtr<Semaphore> MakeSemaphore() override;

    public:
        [[nodiscard]]
        SharedPtr<CommandPool> CreateCommandPool(
            const SharedPtr<CommandQueue>& queue) override;

        [[nodiscard]]
        SharedPtr<RenderPipeline> CreateRenderPipeline(
            const RenderPipelineSpecifications& specifications) override;

        [[nodiscard]]
        SharedPtr<ShaderModule> CreateShaderModule(
            const Byte* shaderSource, Usize sourceSize) override;

        [[nodiscard]]
        SharedPtr<TextureView> CreateTextureView(
            const SharedPtr<Texture>& texture,
            const TextureViewSpecifications& specifications) override;

    public:
        [[nodiscard]]
        inline VkPhysicalDevice GetVulkanPhysicalDevice() const
        {
            return m_PhysicalDevice;
        }

        [[nodiscard]]
        inline VkDevice GetVulkanDevice() const
        {
            return m_Device;
        }

    private:
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;

        Array<Array<SharedPtr<VulkanCommandQueue>>> m_CommandQueues;
    };
}
