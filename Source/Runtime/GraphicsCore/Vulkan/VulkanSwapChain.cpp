#include "GraphicsCore/Vulkan/VulkanSwapChain.h"

#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanSemaphore.h"

#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanCommandQueue.h"
#include "GraphicsCore/Vulkan/VulkanRenderSurface.h"

#include "Foundation/Algorithms/Contains.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    namespace Details
    {
        VkPresentModeKHR ToVkPresentMode_(PresentMode presentMode)
        {
            switch (presentMode)
            {
            case PresentMode::Immediate:
                return VK_PRESENT_MODE_IMMEDIATE_KHR;
            case PresentMode::Fifo:
                return VK_PRESENT_MODE_FIFO_KHR;
            case PresentMode::Mailbox:
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }

            KITSUNE_UNREACHABLE();
        }
    }

    // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
    // False alarm, the swap chain properties are set by RecreateSwapChain_().
    VulkanSwapChain::VulkanSwapChain(
        VulkanGpuDevice& device,
        const SharedPtr<VulkanRenderSurface>& surface,
        const SharedPtr<VulkanCommandQueue>& presentQueue,
        const SwapChainConfiguration& configuration)
        : m_Device(device),
          m_Surface(surface),
          m_PresentQueue(presentQueue)
    {
        if ((m_Surface == nullptr) || (m_PresentQueue == nullptr))
        {
            throw InvalidArgumentException(
                "Failed to create a Vulkan swap chain. VulkanSwapChain() was called "
                "with either surface and/or present queue set to nullptr.");
        }

        VkPhysicalDevice physicalDevice = m_Device.GetVulkanPhysicalDevice();
        VkSurfaceCapabilitiesKHR capabilities;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                physicalDevice,
                m_Surface->GetVulkanSurface(),
                &capabilities),
            "Failed to get the surface capabilities of the Vulkan "
            "physical device.");

        if ((configuration.ImageCount < capabilities.minImageCount) ||
            (configuration.ImageCount > capabilities.maxImageCount))
        {
            throw InvalidArgumentException(
                "Failed to configure the swap chain. The specified minimum image count "
                "is outside of the capabilities of the surface.");
        }

        if ((configuration.Extent.X < capabilities.minImageExtent.width) ||
            (configuration.Extent.Y < capabilities.minImageExtent.height) ||
            (configuration.Extent.X > capabilities.maxImageExtent.width) ||
            (configuration.Extent.Y > capabilities.maxImageExtent.height))
        {
            throw InvalidArgumentException(
                "Failed to configure the swap chain. The extent given is outside "
                "of the capabilities of the surface.");
        }

        VkPresentModeKHR presentMode = GetVkPresentMode_(
            m_Surface, physicalDevice, configuration.PresentMode);

        VkSurfaceFormatKHR surfaceFormat = GetVkSurfaceFormat_(
            m_Surface, physicalDevice, configuration.Format);

        RecreateSwapChain_(
            surfaceFormat,
            presentMode,
            configuration.ImageCount,
            Details::ToVkExtent2D_(configuration.Extent),
            capabilities.currentTransform);
    }
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)

    VulkanSwapChain::~VulkanSwapChain()
    {
        ::vkDestroySwapchainKHR(m_Device.GetVulkanDevice(), m_SwapChain, nullptr);
    }

    Pair<Uint32, bool> VulkanSwapChain::AcquireNextImage(
        const SharedPtr<Semaphore>& waitedSemaphore)
    {
        Uint32 index = 0;
        VkSemaphore vulkanSemaphore = (waitedSemaphore != nullptr) ?
            Details::ToImplementation_(waitedSemaphore)->GetVulkanSemaphore() :
            VK_NULL_HANDLE;

        VkResult result = ::vkAcquireNextImageKHR(
            m_Device.GetVulkanDevice(), m_SwapChain, UINT64_MAX,
            vulkanSemaphore, nullptr, &index);

        if ((result < 0) && (result != VK_ERROR_OUT_OF_DATE_KHR))
            throw SystemException("Failed to acquire an image from the swap chain.");

        // VK_SUBOPTIMAL_KHR is considered a success.
        return { index, (result >= 0) };
    }

    void VulkanSwapChain::Present(Uint32 index, const SharedPtr<Semaphore>& semaphore)
    {
        VkSemaphore waited = (semaphore != nullptr) ?
            Details::ToImplementation_(semaphore)->GetVulkanSemaphore() :
            VK_NULL_HANDLE;

        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = (waited != VK_NULL_HANDLE),
            .pWaitSemaphores = &waited,
            .swapchainCount = 1,
            .pSwapchains = &m_SwapChain,
            .pImageIndices = &index,
            .pResults = nullptr
        };

        VkResult result = ::vkQueuePresentKHR(
            m_PresentQueue->GetVulkanQueue(),
            &presentInfo);

        // Let AcquireNextImage() handle outdated/suboptimal cases.
        if ((result < 0) && (result != VK_ERROR_OUT_OF_DATE_KHR))
            throw SystemException("Failed to present a surface texture.");
    }

    void VulkanSwapChain::Resize(const Vector2<Uint32>& newSize)
    {
        RecreateSwapChain_(
            m_SurfaceFormat,
            m_PresentMode,
            m_ImageCount,
            Details::ToVkExtent2D_(newSize),
            m_Transform);
    }

    SwapChainConfiguration VulkanSwapChain::GetConfiguration() const
    {
        PresentMode presentMode;
        switch (m_PresentMode)
        {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            presentMode = PresentMode::Immediate;
            break;
        case VK_PRESENT_MODE_FIFO_KHR:
            presentMode = PresentMode::Fifo;
            break;
        case VK_PRESENT_MODE_MAILBOX_KHR:
            presentMode = PresentMode::Mailbox;
            break;
        default:
            KITSUNE_UNREACHABLE();
        }

        return {
            .ImageCount = m_ImageCount,
            .Extent = Details::ToVector2_(m_Extent),
            .Format = Details::ToTextureFormat_(m_SurfaceFormat.format),
            .PresentMode = presentMode
        };
    }

    void VulkanSwapChain::RecreateSwapChain_(
        VkSurfaceFormatKHR surfaceFormat,
        VkPresentModeKHR presentMode,
        Uint32 imageCount,
        const VkExtent2D& imageExtent,
        VkSurfaceTransformFlagBitsKHR transform)
    {
        if (m_SwapChain != nullptr)
        {
            m_BackBuffers.Clear();
            ::vkDestroySwapchainKHR(m_Device.GetVulkanDevice(), m_SwapChain, nullptr);
        }

        VkSwapchainCreateInfoKHR createInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = m_Surface->GetVulkanSurface(),
            .minImageCount = imageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = imageExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .preTransform = transform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = true,
            .oldSwapchain = VK_NULL_HANDLE
        };

        VkDevice device = m_Device.GetVulkanDevice();
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain),
            "Failed to create a swap chain for the window.");

        Uint32 backBufferCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetSwapchainImagesKHR(device, m_SwapChain, &backBufferCount, nullptr),
            "Failed to retrieve a Vulkan swap chain's backbuffers.");

        Array<VkImage> backBuffers(backBufferCount, VkImage());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetSwapchainImagesKHR(
                device, m_SwapChain, &backBufferCount, backBuffers.Data()),
            "Failed to retrieve a Vulkan swap chain's backbuffers.");

        // The stored back buffers are of type VulkanTexture, we have to create
        // a temporary array to house the VkImages first.
        for (VkImage backBuffer : backBuffers)
        {
            m_BackBuffers.PushBack(
                MakeScoped<VulkanTexture>(
                    backBuffer,
                    Details::ToVector2_(imageExtent)));
        }

        m_SurfaceFormat = surfaceFormat;
        m_PresentMode = presentMode;
        m_ImageCount = backBufferCount;
        m_Extent = imageExtent;
        m_Transform = transform;
    }

    [[nodiscard]]
    VkPresentModeKHR VulkanSwapChain::GetVkPresentMode_(
        const SharedPtr<VulkanRenderSurface>& surface,
        VkPhysicalDevice physicalDevice,
        PresentMode presentMode)
    {
        VkPresentModeKHR vulkanPresentMode = Details::ToVkPresentMode_(presentMode);
        Array<VkPresentModeKHR> presentModes =
            surface->GetPhysicalDeviceSurfacePresentModes(physicalDevice);

        if (Algorithms::Contains(presentModes.GetBegin(), presentModes.GetEnd(),
                                 vulkanPresentMode))
        {
            return vulkanPresentMode;
        }

        throw SystemException(
            "Failed to construct the Vulkan swap chain. The specified presentation mode "
            "is not supported.");
    }

    [[nodiscard]]
    VkSurfaceFormatKHR VulkanSwapChain::GetVkSurfaceFormat_(
        const SharedPtr<VulkanRenderSurface>& surface,
        VkPhysicalDevice physicalDevice,
        TextureFormat format)
    {
        VkFormat vulkanFormat = Details::ToVkFormat_(format);
        Array<VkSurfaceFormatKHR> supportedFormats =
            surface->GetPhysicalDeviceSurfaceFormats(physicalDevice);

        const auto predicate = [&](const VkSurfaceFormatKHR& surfaceFormat) -> bool
        {
            return (surfaceFormat.format == vulkanFormat);
        };

        auto formatIter = Algorithms::FindIf(
            supportedFormats.GetBegin(), supportedFormats.GetEnd(),
            predicate);

        if (formatIter == supportedFormats.GetEnd())
        {
            throw SystemException(
                "Failed to construct the Vulkan swap chain. The specified format "
                "is not supported.");
        }

        return *formatIter;
    }
}
