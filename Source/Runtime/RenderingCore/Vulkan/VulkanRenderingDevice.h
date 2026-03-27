#pragma once

#include "Application/Window.h"
#include "Foundation/Containers/Array.h"

#include "RenderingCore/RenderingDevice.h"
#include "RenderingCore/Vulkan/VulkanHeader.h"

namespace Kitsune
{
    class VulkanRenderingContext;

    struct VulkanSwapChain
    {
        VkSwapchainKHR Handle;
        VkSurfaceFormatKHR SurfaceFormat;
        VkExtent2D Extents;

        Array<VkImageView> ImageViews;
    };

    class VulkanRenderingDevice : public RenderingDevice
    {
    public:
        VulkanRenderingDevice(VkInstance instance,
                              VkPhysicalDevice physicalDevice,
                              WindowHandle windowHandle);

        ~VulkanRenderingDevice() override;

    public:
        [[nodiscard]]
        RenderingDeviceInformation GetInformation() const override;

    private:
        // Will call the platform-specific creation functions, for example
        // vkCreateWin32SurfaceKHR() and vkCreateXcbSurfaceKHR()
        void InitializePlatformSurface_(WindowHandle windowHandle);

    private:
        [[nodiscard]] static Array<const char*> GetRequestedExtensions_();
        void VerifyExtensionsSupport_(const Array<const char*>& extensions);

        [[nodiscard]]
        Array<VkDeviceQueueCreateInfo> GetDeviceQueueCreateInfo_() const;

    private:
        [[nodiscard]] VkSurfaceFormatKHR GetSwapchainSurfaceFormat_() const;
        [[nodiscard]] VkPresentModeKHR GetSwapchainPresentMode_() const;

        [[nodiscard]]
        static std::uint32_t GetSwapchainMinImageCount_(
            const VkSurfaceCapabilitiesKHR& capabilities);

        [[nodiscard]]
        static VkExtent2D GetSwapchainExtents_(
            const VkSurfaceCapabilitiesKHR& capabilities,
            WindowHandle windowHandle);

        void InitializeSwapchain_(WindowHandle windowHandle);
        void DestroySwapchain_();

    private:
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;

        VkDevice m_Device = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_ComputeQueue = VK_NULL_HANDLE;
        VkQueue m_TransferQueue = VK_NULL_HANDLE;

        VulkanSwapChain m_SwapChain;
    };
}
