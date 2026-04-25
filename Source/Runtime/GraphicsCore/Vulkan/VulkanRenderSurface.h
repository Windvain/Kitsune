#pragma once

#include "GraphicsCore/RenderSurface.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]] SurfacePresentMode VulkanToEngine_(VkPresentModeKHR presentMode);
        [[nodiscard]] VkPresentModeKHR EngineToVulkan_(SurfacePresentMode presentMode);

        [[nodiscard]]
        VkSurfaceKHR GetVulkanHandle_(const SharedPtr<RenderSurface>& surface);
    }

    class Window;

    class VulkanGpuInstance;
    class VulkanGpuDevice;
    class VulkanTexture;

    class VulkanRenderSurface : public RenderSurface
    {
    public:
        inline VulkanRenderSurface(VulkanGpuInstance& instance, Window* window)
            : m_Instance(instance)
        {
            if (window == nullptr)
            {
                throw SystemException(
                    "Tried to construct a RenderSurface with an invalid window.");
            }

            // Hand over the responsibility of creating the Vulkan surface
            // over to the platform-specific function.
            CreatePlatformVulkanSurface_(window);
        }

        ~VulkanRenderSurface() override;

    public:
        [[nodiscard]]
        RenderSurfaceCapabilities GetCapabilities(
            const SharedPtr<GpuDevice>& device) const override;

        [[nodiscard]]
        inline SwapChainConfiguration GetSwapChainConfiguration() const override
        {
            return m_SwapChainConfiguration;
        }

        [[nodiscard]]
        inline SharedPtr<Texture> GetBackBuffer(Uint32 index) const override;

    public:
        [[nodiscard]]
        Uint32 AcquireNextImage(SharedPtr<Semaphore>& semaphore) override;

    public:
        void ConfigureSwapChain(
            const SharedPtr<GpuDevice>& device,
            const SwapChainConfiguration& configuration) override;

    public:
        [[nodiscard]]
        inline VkSurfaceKHR GetVulkanSurface() const
        {
            return m_Surface;
        }

        [[nodiscard]]
        inline VkSwapchainKHR GetVulkanSwapChain() const
        {
            return m_SwapChain;
        }

    private:
        // Implemented by each operating system implementation.
        void CreatePlatformVulkanSurface_(Window* window);

        [[nodiscard]]
        VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities_(
            VkPhysicalDevice physicalDevice) const;

        [[nodiscard]]
        Array<SurfacePresentMode> GetSupportedPresentationModes_(
            VkPhysicalDevice physicalDevice) const;

        [[nodiscard]]
        Array<TextureFormat> GetSupportedFormats_(
            VkPhysicalDevice physicalDevice) const;

    private:
        VulkanGpuInstance& m_Instance;
        SharedPtr<VulkanGpuDevice> m_Device = nullptr;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        SwapChainConfiguration m_SwapChainConfiguration;

        Array<SharedPtr<VulkanTexture>> m_BackBuffers;
    };
}
