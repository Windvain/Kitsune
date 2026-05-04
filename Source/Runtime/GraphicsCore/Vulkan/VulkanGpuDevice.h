#pragma once

#include "GraphicsCore/GpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    class VulkanCommandQueue;

    class VulkanGpuDevice : public GpuDevice
    {
    public:
        VulkanGpuDevice(
            VkPhysicalDevice physicalDevice,
            const Array<VkDeviceQueueCreateInfo>& queueInfos,
            const Array<const char*>& extensions,
            GpuDeviceFeature features);

        ~VulkanGpuDevice() override;

    public:
        [[nodiscard]]
        SharedPtr<CommandQueue> GetCommandQueue(
            Uint32 index, Uint32 queueIndex) const override;

        [[nodiscard]]
        inline GpuDeviceFeature GetFeatures() const override
        {
            return m_Features;
        }

    public:
        [[nodiscard]]
        SharedPtr<GpuBuffer> CreateBuffer(
            const GpuBufferSpecifications& specifications) override;

        [[nodiscard]]
        SharedPtr<CommandPool> CreateCommandPool(
            const SharedPtr<CommandQueue>& commandQueue) override;

        [[nodiscard]]
        SharedPtr<Fence> CreateFence(FenceFlag flags) override;

        [[nodiscard]]
        SharedPtr<RenderPipeline> CreateRenderPipeline(
            const RenderPipelineSpecifications& specifications) override;

        [[nodiscard]]
        SharedPtr<Semaphore> MakeSemaphore() override;

        [[nodiscard]]
        SharedPtr<SwapChain> CreateSwapChain(
            const SharedPtr<RenderSurface>& surface,
            const SharedPtr<CommandQueue>& presentQueue,
            const SwapChainConfiguration& configuration) override;

        [[nodiscard]]
        SharedPtr<TextureView> CreateTextureView(
            const SharedPtr<Texture>& texture,
            const TextureViewSpecifications& specifications) override;

    public:
        [[nodiscard]]
        SharedPtr<ShaderModule> CreateShaderModule(Array<Byte>&& shaderCode) override;

    public:
        [[nodiscard]]
        VkDeviceMemory AllocateMemory(
            VkDeviceSize allocSize,
            Uint32 supportedMemoryTypes,
            VkMemoryPropertyFlags flags);

        void FreeMemory(VkDeviceMemory deviceMemory);

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
        [[nodiscard]]
        Uint32 GetMemoryTypeIndex_(Uint32 typeFilter, VkMemoryPropertyFlags flags);

    private:
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;

        VkPhysicalDeviceMemoryProperties m_MemoryProperties{ /* ... */ };

        GpuDeviceFeature m_Features;
        Array<Array<SharedPtr<VulkanCommandQueue>>> m_CommandQueues;
    };
}
