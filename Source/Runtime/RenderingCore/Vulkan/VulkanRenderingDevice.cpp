#include "RenderingCore/Vulkan/VulkanRenderingDevice.h"
#include "RenderingCore/Vulkan/VulkanRenderingContext.h"

#include "Foundation/Containers/Set.h"
#include "Foundation/Logging/Logger.h"

#include "Foundation/Maths/Clamp.h"
#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Algorithms/Contains.h"

namespace Kitsune
{
    VulkanRenderingDevice::VulkanRenderingDevice(
        VkInstance instance,
        VkPhysicalDevice physicalDevice,
        WindowHandle windowHandle)
        : m_Instance(instance), m_PhysicalDevice(physicalDevice)
    {
        InitializePlatformSurface_(windowHandle);

        auto queueCreateInfos = GetDeviceQueueCreateInfo_();
        Array<const char*> deviceExtensions = GetRequestedExtensions_();

        VerifyExtensionsSupport_(deviceExtensions);
        VkPhysicalDeviceFeatures deviceFeatures = { /* ... */ };

        VkDeviceCreateInfo deviceCreateInfo = { /* ... */ };
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.Data();
        deviceCreateInfo.queueCreateInfoCount =
            static_cast<std::uint32_t>(queueCreateInfos.Size());

        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.Data();
        deviceCreateInfo.enabledExtensionCount =
            static_cast<std::uint32_t>(deviceExtensions.Size());

        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateDevice(
                physicalDevice, &deviceCreateInfo, nullptr, &m_Device),
            "Failed to create a Vulkan logical device.");

        ::vkGetDeviceQueue(
            m_Device, queueCreateInfos[0].queueFamilyIndex,
            0, &m_GraphicsQueue);

        ::vkGetDeviceQueue(
            m_Device, queueCreateInfos[1].queueFamilyIndex,
            0, &m_ComputeQueue);

        ::vkGetDeviceQueue(
            m_Device, queueCreateInfos[2].queueFamilyIndex,
            0, &m_TransferQueue);

        InitializeSwapchain_(windowHandle);
    }

    VulkanRenderingDevice::~VulkanRenderingDevice()
    {
        KITSUNE_ASSERT(
            m_Device != VK_NULL_HANDLE,
            "Tried to destroy a logical device which had not been "
            "initialized.");

        DestroySwapchain_();

        ::vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        ::vkDestroyDevice(m_Device, nullptr);
    }

    RenderingDeviceInformation VulkanRenderingDevice::GetInformation() const
    {
        VkPhysicalDeviceProperties deviceProperties;
        ::vkGetPhysicalDeviceProperties(
            m_PhysicalDevice, &deviceProperties);

        RenderingDeviceInformation deviceInfo;
        deviceInfo.Name = deviceProperties.deviceName;

        switch (deviceProperties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            deviceInfo.Type = RenderingDeviceType::Integrated;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            deviceInfo.Type = RenderingDeviceType::Discrete;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            deviceInfo.Type = RenderingDeviceType::Virtual;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_CPU:
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        default:        // VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM
            deviceInfo.Type = RenderingDeviceType::Other;
            break;
        }

        return deviceInfo;
    }

    Array<const char*> VulkanRenderingDevice::GetRequestedExtensions_()
    {
        Array<const char*> requestedExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME     // For VkSwapchainKHR.
        };

        return requestedExtensions;
    }

    void VulkanRenderingDevice::VerifyExtensionsSupport_(
        const Array<const char*>& requestedExtensions)
    {
        std::uint32_t extensionCount;
        Array<VkExtensionProperties> propertiesArray;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateDeviceExtensionProperties(
                m_PhysicalDevice, nullptr, &extensionCount, nullptr),
            "Failed to enumerate through the Vulkan physical device's "
            "supported extensions.");

        propertiesArray.Resize(extensionCount);
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateDeviceExtensionProperties(
                m_PhysicalDevice, nullptr, &extensionCount,
                propertiesArray.Data()),
            "Failed to enumerate through the Vulkan physical device's "
            "supported extensions.");

        for (const char* extension : requestedExtensions)
        {
            const auto predicate =
                [extension](const VkExtensionProperties& properties) -> bool
                {
                    return (StringView(properties.extensionName) == extension);
                };

            if (!Algorithms::ContainsIf(
                    propertiesArray.GetBegin(), propertiesArray.GetEnd(),
                    predicate))
            {
                throw SystemException(
                    "The specified physical device does not support one or "
                    "more of the engine's requested device extensions.");
            }
        }
    }

    Array<VkDeviceQueueCreateInfo>
    VulkanRenderingDevice::GetDeviceQueueCreateInfo_() const
    {
        std::uint32_t queueFamilyCount = 0;
        Array<VkQueueFamilyProperties> queueFamilyProperties;

        ::vkGetPhysicalDeviceQueueFamilyProperties(
            m_PhysicalDevice, &queueFamilyCount,
            nullptr);

        queueFamilyProperties.Resize(queueFamilyCount);
        ::vkGetPhysicalDeviceQueueFamilyProperties(
            m_PhysicalDevice, &queueFamilyCount,
            queueFamilyProperties.Data());

        Set<std::uint32_t> indices;
        const VkQueueFlags queuesToFind[3] = {
            VK_QUEUE_GRAPHICS_BIT,
            VK_QUEUE_COMPUTE_BIT,
            VK_QUEUE_TRANSFER_BIT
        };

        Array<VkDeviceQueueCreateInfo> createInfoArray;
        static const float queuePriorities[] = { 1.0f };

        VkDeviceQueueCreateInfo createInfo = { /* ... */ };
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createInfo.queueCount = 1;
        createInfo.pQueuePriorities = queuePriorities;

        for (VkQueueFlags flags : queuesToFind)
        {
            std::uint32_t index = 0;
            for (; index < queueFamilyCount; ++index)
            {
                const auto& properties = queueFamilyProperties[index];
                if (!(properties.queueFlags & flags))
                    continue;

                auto [iter_, result] = indices.Insert(index);
                if (result)
                {
                    createInfo.queueFamilyIndex = index;
                    createInfoArray.PushBack(createInfo);

                    break;
                }
            }

            std::string queueString = string_VkQueueFlags(flags);
            std::string flagsString =
                string_VkQueueFlags(queueFamilyProperties[index].queueFlags);

            KITSUNE_ENGINE_INFO_FORMAT(
                Rendering,
                "Found device queue family #{0} for {1} queue: {2}",
                index,
                queueString.c_str(),
                flagsString.c_str());
        }

        if (createInfoArray.Size() != 3)
        {
            throw SystemException(
                "Could not find three unique queue families for "
                "the Graphics, Compute, and Transfer queues.");
        }

        // Query for presentation support.
        VkBool32 supportsPresentation;
        std::uint32_t graphicsIndex = createInfoArray[0].queueFamilyIndex;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceSupportKHR(
                m_PhysicalDevice, graphicsIndex, m_Surface,
                &supportsPresentation),
            "Failed to retrieve the presentation state of a Vulkan "
            "physical device.");

        if (!supportsPresentation)
        {
            throw SystemException(
                "The graphics queue does not support presentation.");
        }

        return createInfoArray;
    }

    VkSurfaceFormatKHR VulkanRenderingDevice::GetSwapchainSurfaceFormat_() const
    {
        std::uint32_t surfaceFormatsCount;
        Array<VkSurfaceFormatKHR> surfaceFormats;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_PhysicalDevice, m_Surface, &surfaceFormatsCount,
                nullptr),
            "Failed to get the surface formats supported by the "
            "Vulkan physical device.");

        surfaceFormats.Resize(surfaceFormatsCount);
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_PhysicalDevice, m_Surface, &surfaceFormatsCount,
                surfaceFormats.Data()),
            "Failed to get the surface formats which are supported by "
            "the Vulkan physical device.");

        VkSurfaceFormatKHR requestedFormat = {
            .format = VK_FORMAT_B8G8R8A8_SRGB,
            .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        };

        const auto predicate =
            [&](const VkSurfaceFormatKHR& supportedFormat) -> bool
            {
                return (requestedFormat.format == supportedFormat.format) &&
                       (requestedFormat.colorSpace == supportedFormat.colorSpace);
            };

        if (!Algorithms::ContainsIf(
                surfaceFormats.GetBegin(), surfaceFormats.GetEnd(),
                predicate))
        {
            throw SystemException(
                "The physical device doesn't support the B8G8R8A8 pixel "
                "format in the sRGB color space.");
        }

        return requestedFormat;
    }

    VkPresentModeKHR VulkanRenderingDevice::GetSwapchainPresentMode_() const
    {
        std::uint32_t presentModesCount;
        Array<VkPresentModeKHR> presentModes;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfacePresentModesKHR(
                    m_PhysicalDevice, m_Surface, &presentModesCount,
                    nullptr),
            "Failed to get the present modes supported by the Vulkan "
            "physical device.");

        presentModes.Resize(presentModesCount);
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfacePresentModesKHR(
                    m_PhysicalDevice, m_Surface, &presentModesCount,
                    presentModes.Data()),
            "Failed to get the present modes which are supported by the "
            "Vulkan physical device.");

        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (!Algorithms::Contains(
                presentModes.GetBegin(), presentModes.GetEnd(),
                presentMode))
        {
            throw SystemException(
                "The Vulkan physical device used does not support the FIFO "
                "(First In First Out) present mode.");
        }

        return presentMode;
    }

    std::uint32_t VulkanRenderingDevice::GetSwapchainMinImageCount_(
        const VkSurfaceCapabilitiesKHR& capabilities)
    {
        const std::uint32_t imageCount = 3;

        // If capabilities.maxImageCount == 0, then the device does not limit
        // the amount of images in a swap chain, instead, the amount of images
        // is limited by the device memory.
        bool exceedsMaxImageCount = (capabilities.maxImageCount != 0) &&
                                    (imageCount > capabilities.maxImageCount);

        bool belowMinImageCount = (imageCount < capabilities.minImageCount);
        if (belowMinImageCount || exceedsMaxImageCount)
        {
            throw SystemException(
                "The Vulkan physical device cannot handle more than 3 "
                "buffers in the swap chain.");
        }

        return imageCount;
    }

    VkExtent2D VulkanRenderingDevice::GetSwapchainExtents_(
        const VkSurfaceCapabilitiesKHR& capabilities,
        WindowHandle windowHandle)
    {
        // If capabilities.currentExtent is filled with the value 0xFFFFFFFF,
        // then the surface size will be determined by the extent of a swap
        // chain targeting the surface.
        if (capabilities.currentExtent.width != std::uint32_t(-1))
            return capabilities.currentExtent;

        Vector2<Uint32> size = windowHandle->GetSize();
        VkExtent2D minImageExtent = capabilities.minImageExtent;
        VkExtent2D maxImageExtent = capabilities.maxImageExtent;

        return {
            Maths::Clamp(size.X, minImageExtent.width, maxImageExtent.width),
            Maths::Clamp(size.Y, minImageExtent.height, maxImageExtent.height)
        };
    }

    void VulkanRenderingDevice::InitializeSwapchain_(WindowHandle windowHandle)
    {
#pragma region Swap Chain Creation
        VkSurfaceCapabilitiesKHR capabilities;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                m_PhysicalDevice, m_Surface, &capabilities),
            "Failed to get the surface capabilities of the Vulkan "
            "physical device.");

        VkSurfaceFormatKHR surfaceFormat = GetSwapchainSurfaceFormat_();
        VkPresentModeKHR presentMode = GetSwapchainPresentMode_();

        VkExtent2D extents = GetSwapchainExtents_(capabilities, windowHandle);
        std::uint32_t minImageCount = GetSwapchainMinImageCount_(capabilities);

        VkSwapchainCreateInfoKHR swapChainCreateInfo = { /* ... */ };
        swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapChainCreateInfo.surface = m_Surface;
        swapChainCreateInfo.minImageCount = minImageCount;
        swapChainCreateInfo.imageFormat = surfaceFormat.format;
        swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapChainCreateInfo.imageExtent = extents;
        swapChainCreateInfo.imageArrayLayers = 1;
        swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapChainCreateInfo.preTransform = capabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainCreateInfo.presentMode = presentMode;
        swapChainCreateInfo.clipped = true;
        swapChainCreateInfo.oldSwapchain = nullptr;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateSwapchainKHR(m_Device, &swapChainCreateInfo, nullptr,
                                   &m_SwapChain.Handle),
            "Failed to create a swap chain for the window.");
#pragma endregion

#pragma region Image View Creation
        m_SwapChain.Extents = extents;
        m_SwapChain.SurfaceFormat = surfaceFormat;

        std::uint32_t swapChainImageCount;
        Array<VkImage> swapChainImages;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetSwapchainImagesKHR(
                m_Device, m_SwapChain.Handle, &swapChainImageCount, nullptr),
            "Failed to retrieve the swap chain's image handles.");

        swapChainImages.Resize(swapChainImageCount);
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetSwapchainImagesKHR(
                m_Device, m_SwapChain.Handle, &swapChainImageCount,
                swapChainImages.Data()),
            "Failed to retrieve the swap chain's image handles.");

        VkImageViewCreateInfo imageViewCreateInfo = { /* ... */ };
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = m_SwapChain.SurfaceFormat.format;
        imageViewCreateInfo.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        };

        imageViewCreateInfo.subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        for (VkImage image : swapChainImages)
        {
            imageViewCreateInfo.image = image;

            VkImageView imageView;
            KITSUNE_VK_THROW_IF_FAIL(
                ::vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr,
                                    &imageView),
                "Failed to create an image view for the swap chain!");

            m_SwapChain.ImageViews.PushBack(imageView);
        }
#pragma endregion

        KITSUNE_ENGINE_INFO_FORMAT(
            Rendering,
            "Initialized the Vulkan swap chain. [ Format: {0}, Color space: "
            "{1}, Present mode: {2}, Extents: {3} x {4}, Framebuffers: {5}",
            string_VkFormat(surfaceFormat.format),
            string_VkColorSpaceKHR(surfaceFormat.colorSpace),
            string_VkPresentModeKHR(presentMode),
            extents.width, extents.height,
            minImageCount);
    }

    void VulkanRenderingDevice::DestroySwapchain_()
    {
        for (VkImageView imageView : m_SwapChain.ImageViews)
            ::vkDestroyImageView(m_Device, imageView, nullptr);

        ::vkDestroySwapchainKHR(m_Device, m_SwapChain.Handle, nullptr);
    }
}
