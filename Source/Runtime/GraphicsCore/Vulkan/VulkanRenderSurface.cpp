#include "GraphicsCore/Vulkan/VulkanRenderSurface.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanGpuInstance.h"

#include "Foundation/Diagnostics/InvalidArgumentException.h"

#if defined(KITSUNE_OS_WINDOWS)
    #include "Application/Windows/WindowsWindow.h"
#endif

namespace Kitsune
{
    namespace Details
    {
        SharedPtr<VulkanRenderSurface> ToImplementation_(
            const SharedPtr<RenderSurface>& surface)
        {
            return DynamicPointerCast<VulkanRenderSurface>(surface);
        }
    }

    VulkanRenderSurface::VulkanRenderSurface(VulkanGpuInstance& instance,
                                             Window* window)
        : m_Instance(instance)
    {
        if (window == nullptr)
        {
            throw SystemException(
                "Failed to construct a VulkanRenderSurface. The window passed into "
                "the constructor is invalid. (NULL)");
        }

#if defined(KITSUNE_OS_WINDOWS)
        auto* windowsWindow = dynamic_cast<WindowsWindow*>(window);
        HWND hwnd = windowsWindow->GetNativeHandle();

        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = nullptr,
            .hwnd = hwnd
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateWin32SurfaceKHR(
                m_Instance.GetVulkanInstance(), &surfaceCreateInfo,
                nullptr, &m_Surface),
            "Failed to create a Win32 surface from the newly created window.");
#else
    #error Could not find an implementation for creating a Vulkan surface.
#endif
    }

    VulkanRenderSurface::~VulkanRenderSurface()
    {
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

        VkSurfaceCapabilitiesKHR capabilities;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                physicalDevice,
                m_Surface,
                &capabilities),
            "Failed to get the surface capabilities of the Vulkan "
            "physical device.");

        Array<TextureFormat> formats;
        Array<VkSurfaceFormatKHR> vulkanSurfaceFormats =
            GetPhysicalDeviceSurfaceFormats(physicalDevice);

        for (VkSurfaceFormatKHR vulkanSurfaceFormat : vulkanSurfaceFormats)
        {
            TextureFormat format = Details::ToTextureFormat_(vulkanSurfaceFormat.format);
            if (format != TextureFormat::Unknown)
                formats.PushBack(format);
        }

        Array<PresentMode> presentModes;
        Array<VkPresentModeKHR> vulkanPresentModes =
            GetPhysicalDeviceSurfacePresentModes(physicalDevice);

        for (VkPresentModeKHR vulkanPresentMode : vulkanPresentModes)
        {
            switch (vulkanPresentMode)
            {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                presentModes.PushBack(PresentMode::Immediate);
                break;
            case VK_PRESENT_MODE_FIFO_KHR:
                presentModes.PushBack(PresentMode::Fifo);
                break;
            case VK_PRESENT_MODE_MAILBOX_KHR:
                presentModes.PushBack(PresentMode::Mailbox);
                break;
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:              [[fallthrough]];
            case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:     [[fallthrough]];
            case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: [[fallthrough]];
            case VK_PRESENT_MODE_FIFO_LATEST_READY_KHR:         [[fallthrough]];
            default:
                break;
            }
        }

        return {
            .MinimumImageCount = capabilities.minImageCount,
            .MaximumImageCount = capabilities.maxImageCount,
            .PresentModes = presentModes,
            .TextureFormats = formats,
            .MinimumExtents = Details::ToVector2_(capabilities.minImageExtent),
            .MaximumExtents = Details::ToVector2_(capabilities.maxImageExtent),
        };
    }

    Array<VkSurfaceFormatKHR> VulkanRenderSurface::GetPhysicalDeviceSurfaceFormats(
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

        return surfaceFormats;
    }

    Array<VkPresentModeKHR> VulkanRenderSurface::GetPhysicalDeviceSurfacePresentModes(
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

        return presentModes;
    }
}
