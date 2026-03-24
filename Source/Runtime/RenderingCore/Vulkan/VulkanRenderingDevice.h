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

        ~VulkanRenderingDevice();

    public:
        RenderingDeviceInformation GetInformation() const override;

    private:
        // Will call the platform-specific creation functions, for example
        // vkCreateWin32SurfaceKHR() and vkCreateXcbSurfaceKHR()
        void InitializePlatformSurface_(WindowHandle windowHandle);

    private:
        Array<const char*> GetRequestedExtensions_() const;
        void VerifyExtensionsSupport_(const Array<const char*>& extensions);

        Array<VkDeviceQueueCreateInfo> GetDeviceQueueCreateInfo_(VkPhysicalDevice physicalDevice) const;

        VkSurfaceFormatKHR GetSwapchainSurfaceFormat_() const;
        VkPresentModeKHR GetSwapchainPresentMode_() const;

        std::uint32_t GetSwapchainMinimumImageCount_(const VkSurfaceCapabilitiesKHR& capabilities) const;
        VkExtent2D GetSwapchainExtents(const VkSurfaceCapabilitiesKHR& capabilities,
                                       WindowHandle windowHandle) const;

        void InitializeSwapchain_(WindowHandle windowHandle);
        void DestroySwapchain_();

    private:
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;

        VkDevice m_Device;
        VkSurfaceKHR m_Surface;

        VkQueue m_GraphicsQueue;
        VkQueue m_ComputeQueue;
        VkQueue m_TransferQueue;

        VulkanSwapChain m_SwapChain;
    };
}
