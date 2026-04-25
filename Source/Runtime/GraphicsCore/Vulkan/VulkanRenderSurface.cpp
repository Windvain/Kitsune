#include "GraphicsCore/Vulkan/VulkanRenderSurface.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

#include "GraphicsCore/Vulkan/VulkanSemaphore.h"
#include "GraphicsCore/Vulkan/VulkanGpuInstance.h"

#include "Foundation/Algorithms/Contains.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    namespace Details
    {
        VkPresentModeKHR EngineToVulkan_(SurfacePresentMode presentMode)
        {
            switch (presentMode)
            {
            case SurfacePresentMode::Immediate:
                return VK_PRESENT_MODE_IMMEDIATE_KHR;
            case SurfacePresentMode::Fifo:
                return VK_PRESENT_MODE_FIFO_KHR;
            case SurfacePresentMode::Mailbox:
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }

            KITSUNE_UNREACHABLE();
        }

        SurfacePresentMode VulkanToEngine_(VkPresentModeKHR presentMode)
        {
            switch (presentMode)
            {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                return SurfacePresentMode::Immediate;
            case VK_PRESENT_MODE_MAILBOX_KHR:
                return SurfacePresentMode::Mailbox;

            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:      [[fallthrough]];
            case VK_PRESENT_MODE_FIFO_LATEST_READY_KHR: [[fallthrough]];
            case VK_PRESENT_MODE_FIFO_KHR:
                return SurfacePresentMode::Fifo;

            case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
            case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
            case VK_PRESENT_MODE_MAX_ENUM_KHR:
                KITSUNE_UNREACHABLE();
            }

            KITSUNE_UNREACHABLE();
        }

        VkSurfaceKHR GetVulkanHandle_(const SharedPtr<RenderSurface>& surface)
        {
            return DynamicPointerCast<VulkanRenderSurface>(surface)->GetVulkanSurface();
        }
    }

    VulkanRenderSurface::~VulkanRenderSurface()
    {
        if (m_SwapChain != VK_NULL_HANDLE)
        {
            KITSUNE_ASSERT(
                m_Device != nullptr,
                "Cannot destroy the created swap chain because the Vulkan "
                "device was not set.");

            ::vkDestroySwapchainKHR(
                m_Device->GetVulkanDevice(), m_SwapChain, nullptr);
        }

        ::vkDestroySurfaceKHR(m_Instance.GetVulkanInstance(), m_Surface, nullptr);
    }

    RenderSurfaceCapabilities VulkanRenderSurface::GetCapabilities(
        const SharedPtr<GpuDevice>& device) const
    {
        if (device == nullptr)
        {
            throw InvalidArgumentException(
                "Cannot get the capabilities of a render surface without a "
                "GPU device.");
        }

        auto vulkanDevice = DynamicPointerCast<VulkanGpuDevice>(device);
        VkPhysicalDevice physicalDevice = vulkanDevice->GetVulkanPhysicalDevice();

        VkSurfaceCapabilitiesKHR capabilities = GetVkSurfaceCapabilities_(
            physicalDevice);

        return {
            .MinimumImageCount = capabilities.minImageCount,
            .MaximumImageCount = capabilities.maxImageCount,
            .PresentModes = GetSupportedPresentationModes_(physicalDevice),
            .TextureFormats = GetSupportedFormats_(physicalDevice),
            .MinimumExtents = Details::VulkanToEngine_(capabilities.minImageExtent),
            .MaximumExtents = Details::VulkanToEngine_(capabilities.maxImageExtent),
        };
    }

    SharedPtr<Texture> VulkanRenderSurface::GetBackBuffer(Uint32 index) const
    {
        return m_BackBuffers[index];
    }

    Uint32 VulkanRenderSurface::AcquireNextImage(SharedPtr<Semaphore>& semaphore)
    {
        Uint32 index;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkAcquireNextImageKHR(
                m_Device->GetVulkanDevice(), m_SwapChain, UINT64_MAX,
                Details::GetVulkanHandle_(semaphore), nullptr, &index),
            "Failed to acquire the next image from the swap chain.");

        return index;
    }

    void VulkanRenderSurface::ConfigureSwapChain(
        const SharedPtr<GpuDevice>& device,
        const SwapChainConfiguration& configuration)
    {
        m_Device = DynamicPointerCast<VulkanGpuDevice>(device);

        VkPhysicalDevice physicalDevice = m_Device->GetVulkanPhysicalDevice();
        VkSurfaceCapabilitiesKHR capabilities = GetVkSurfaceCapabilities_(
            physicalDevice);

        if ((configuration.ImageCount < capabilities.minImageCount) ||
            (configuration.ImageCount > capabilities.maxImageCount))
        {
            throw InvalidArgumentException(
                "Failed to configure the swap chain. The specified minimum image count "
                "is outside of the capabilities of the surface.");
        }

        if ((configuration.Extents.X < capabilities.minImageExtent.width) ||
            (configuration.Extents.Y < capabilities.minImageExtent.height) ||
            (configuration.Extents.X > capabilities.maxImageExtent.width) ||
            (configuration.Extents.Y > capabilities.maxImageExtent.height))
        {
            throw InvalidArgumentException(
                "Failed to configure the swap chain. The extent given is outside "
                "of the capabilities of the surface.");
        }

        auto supportedPresentModes = GetSupportedPresentationModes_(physicalDevice);
        auto supportedFormats = GetSupportedFormats_(physicalDevice);

        if (!Algorithms::Contains(
            supportedPresentModes.GetBegin(), supportedPresentModes.GetEnd(),
            configuration.PresentMode))
        {
            throw InvalidArgumentException(
                "Failed to configure the swap chain. The presentation mode passed into "
                "VulkanRenderSurface::ConfigureSwapChain() is not supported.");
        }

        if (!Algorithms::Contains(
            supportedFormats.GetBegin(), supportedFormats.GetEnd(),
            configuration.Format))
        {
            throw InvalidArgumentException(
                "Failed to configure the swap chain. The surface format passed into "
                "VulkanRenderSurface::ConfigureSwapChain() is not supported.");
        }

        VkSwapchainCreateInfoKHR createInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = m_Surface,
            .minImageCount = configuration.ImageCount,
            .imageFormat = Details::EngineToVulkan_(configuration.Format),
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = Details::EngineToVulkan_(configuration.Extents),
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = Details::EngineToVulkan_(configuration.PresentMode),
            .clipped = true,
            .oldSwapchain = VK_NULL_HANDLE
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateSwapchainKHR(
                m_Device->GetVulkanDevice(), &createInfo, nullptr,
                &m_SwapChain),
            "Failed to create a swap chain for the window.");

        Uint32 backBufferCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetSwapchainImagesKHR(m_Device->GetVulkanDevice(), m_SwapChain,
                                      &backBufferCount, nullptr),
            "Failed to retrieve a Vulkan swap chain's backbuffers.");

        Array<VkImage> backBuffers(backBufferCount, VkImage());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetSwapchainImagesKHR(m_Device->GetVulkanDevice(), m_SwapChain,
                                      &backBufferCount, backBuffers.Data()),
            "Failed to retrieve a Vulkan swap chain's backbuffers.");

        // The stored back buffers are of type VulkanTexture, we have to create
        // a temporary array to house the VkImages first.
        for (VkImage backBuffer : backBuffers)
            m_BackBuffers.PushBack(MakeScoped<VulkanTexture>(backBuffer));

        m_SwapChainConfiguration = configuration;
    }

    VkSurfaceCapabilitiesKHR VulkanRenderSurface::GetVkSurfaceCapabilities_(
        VkPhysicalDevice physicalDevice) const
    {
        VkSurfaceCapabilitiesKHR capabilities;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                physicalDevice,
                m_Surface,
                &capabilities),
            "Failed to get the surface capabilities of the Vulkan "
            "physical device.");

        return capabilities;
    }

    Array<SurfacePresentMode> VulkanRenderSurface::GetSupportedPresentationModes_(
        VkPhysicalDevice physicalDevice) const
    {
        Uint32 presentModeCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfacePresentModesKHR(
                physicalDevice, m_Surface, &presentModeCount, nullptr),
            "Failed to get the presentation modes supported by the "
            "Vulkan physical device.");

        Array<VkPresentModeKHR> presentModes(presentModeCount, VkPresentModeKHR());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfacePresentModesKHR(
                physicalDevice, m_Surface, &presentModeCount, presentModes.Data()),
            "Failed to get the presentation modes supported by the "
            "Vulkan physical device.");

        Array<SurfacePresentMode> supportedModes;
        for (VkPresentModeKHR presentMode : presentModes)
        {
            switch (presentMode)
            {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                supportedModes.PushBack(SurfacePresentMode::Immediate);
                break;
            case VK_PRESENT_MODE_FIFO_KHR:
                supportedModes.PushBack(SurfacePresentMode::Fifo);
                break;
            case VK_PRESENT_MODE_MAILBOX_KHR:
                supportedModes.PushBack(SurfacePresentMode::Mailbox);
                break;
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:              [[fallthrough]];
            case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:     [[fallthrough]];
            case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: [[fallthrough]];
            case VK_PRESENT_MODE_FIFO_LATEST_READY_KHR:         [[fallthrough]];
            default:
                break;
            }
        }

        return supportedModes;
    }

    Array<TextureFormat> VulkanRenderSurface::GetSupportedFormats_(
        VkPhysicalDevice physicalDevice) const
    {
        Uint32 surfaceFormatCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceFormatsKHR(
                physicalDevice, m_Surface, &surfaceFormatCount, nullptr),
            "Failed to get the surface formats supported by the "
            "Vulkan physical device.");

        Array<VkSurfaceFormatKHR> surfaceFormats(
            surfaceFormatCount, VkSurfaceFormatKHR());

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceFormatsKHR(
                physicalDevice, m_Surface, &surfaceFormatCount, surfaceFormats.Data()),
            "Failed to get the surface formats which are supported by "
            "the Vulkan physical device.");

        Array<TextureFormat> supportedFormats;
        for (VkSurfaceFormatKHR vulkanFormat : surfaceFormats)
        {
            TextureFormat format = Details::VulkanToEngine_(vulkanFormat.format);
            if (format != TextureFormat::Unspecified)
                supportedFormats.PushBack(format);
        }

        return supportedFormats;
    }
}
