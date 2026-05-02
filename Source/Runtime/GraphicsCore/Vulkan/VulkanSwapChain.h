#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "GraphicsCore/SwapChain.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class Texture;

    class VulkanGpuDevice;
    class VulkanCommandQueue;
    class VulkanRenderSurface;

    namespace Details
    {
        [[nodiscard]]
        VkPresentModeKHR ToVkPresentMode_(PresentMode presentMode);
    }

    class VulkanSwapChain : public SwapChain
    {
    public:
        VulkanSwapChain(
            VulkanGpuDevice& device,
            const SharedPtr<VulkanRenderSurface>& surface,
            const SharedPtr<VulkanCommandQueue>& presentQueue,
            const SwapChainConfiguration& configuration);

        ~VulkanSwapChain() override;

    public:
        [[nodiscard]]
        inline SharedPtr<Texture> GetBackBuffer(Uint32 index) const override
        {
            return m_BackBuffers[index];
        }

        [[nodiscard]]
        Pair<Uint32, bool> AcquireNextImage(
            const SharedPtr<Semaphore>& waitedSemaphore) override;

    public:
        void Present(Uint32 index, const SharedPtr<Semaphore>& semaphore) override;
        void Resize(const Vector2<Uint32>& newSize) override;

    public:
        [[nodiscard]]
        SwapChainConfiguration GetConfiguration() const override;

    private:
        void RecreateSwapChain_(
            VkSurfaceFormatKHR surfaceFormat,
            VkPresentModeKHR presentMode,
            Uint32 imageCount,
            const VkExtent2D& imageExtent,
            VkSurfaceTransformFlagBitsKHR transform);

        [[nodiscard]]
        static VkPresentModeKHR GetVkPresentMode_(
            const SharedPtr<VulkanRenderSurface>& surface,
            VkPhysicalDevice physicalDevice,
            PresentMode presentMode);

        [[nodiscard]]
        static VkSurfaceFormatKHR GetVkSurfaceFormat_(
            const SharedPtr<VulkanRenderSurface>& surface,
            VkPhysicalDevice physicalDevice,
            TextureFormat format);

    private:
        VulkanGpuDevice& m_Device;
        SharedPtr<VulkanRenderSurface> m_Surface;
        SharedPtr<VulkanCommandQueue> m_PresentQueue;

        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        Array<SharedPtr<Texture>> m_BackBuffers;

        VkSurfaceFormatKHR m_SurfaceFormat;
        VkPresentModeKHR m_PresentMode;
        Uint32 m_ImageCount;
        VkExtent2D m_Extent;
        VkSurfaceTransformFlagBitsKHR m_Transform;
    };
}
