#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

namespace Kitsune
{
    namespace Details
    {
        TextureFormat VulkanToEngine_(VkFormat format)
        {
            switch (format)
            {
            case VK_FORMAT_R8_UNORM:
                return TextureFormat::R8UnsignedNormalized;
            case VK_FORMAT_R8G8_UNORM:
                return TextureFormat::Rg8UnsignedNormalized;
            case VK_FORMAT_R8G8B8_UNORM:
                return TextureFormat::Rgb8UnsignedNormalized;
            case VK_FORMAT_R8G8B8A8_UNORM:
                return TextureFormat::Rgba8UnsignedNormalized;
            case VK_FORMAT_B8G8R8_UNORM:
                return TextureFormat::Bgr8UnsignedNormalized;
            case VK_FORMAT_B8G8R8A8_UNORM:
                return TextureFormat::Bgra8UnsignedNormalized;
            case VK_FORMAT_R8_SNORM:
                return TextureFormat::R8SignedNormalized;
            case VK_FORMAT_R8G8_SNORM:
                return TextureFormat::Rg8SignedNormalized;
            case VK_FORMAT_R8G8B8_SNORM:
                return TextureFormat::Rgb8SignedNormalized;
            case VK_FORMAT_R8G8B8A8_SNORM:
                return TextureFormat::Rgba8SignedNormalized;
            case VK_FORMAT_B8G8R8_SNORM:
                return TextureFormat::Bgr8SignedNormalized;
            case VK_FORMAT_B8G8R8A8_SNORM:
                return TextureFormat::Bgra8SignedNormalized;
            case VK_FORMAT_R8_UINT:
                return TextureFormat::R8UnsignedInteger;
            case VK_FORMAT_R8G8_UINT:
                return TextureFormat::Rg8UnsignedInteger;
            case VK_FORMAT_R8G8B8_UINT:
                return TextureFormat::Rgb8UnsignedInteger;
            case VK_FORMAT_R8G8B8A8_UINT:
                return TextureFormat::Rgba8UnsignedInteger;
            case VK_FORMAT_B8G8R8_UINT:
                return TextureFormat::Bgr8UnsignedInteger;
            case VK_FORMAT_B8G8R8A8_UINT:
                return TextureFormat::Bgra8UnsignedInteger;
            case VK_FORMAT_R8_SINT:
                return TextureFormat::R8SignedInteger;
            case VK_FORMAT_R8G8_SINT:
                return TextureFormat::Rg8SignedInteger;
            case VK_FORMAT_R8G8B8_SINT:
                return TextureFormat::Rgb8SignedInteger;
            case VK_FORMAT_R8G8B8A8_SINT:
                return TextureFormat::Rgba8SignedInteger;
            case VK_FORMAT_B8G8R8_SINT:
                return TextureFormat::Bgr8SignedInteger;
            case VK_FORMAT_B8G8R8A8_SINT:
                return TextureFormat::Bgra8SignedInteger;
            case VK_FORMAT_R8_SRGB:
                return TextureFormat::R8Srgb;
            case VK_FORMAT_R8G8_SRGB:
                return TextureFormat::Rg8Srgb;
            case VK_FORMAT_R8G8B8_SRGB:
                return TextureFormat::Rgb8Srgb;
            case VK_FORMAT_R8G8B8A8_SRGB:
                return TextureFormat::Rgba8Srgb;
            case VK_FORMAT_B8G8R8_SRGB:
                return TextureFormat::Bgr8Srgb;
            case VK_FORMAT_B8G8R8A8_SRGB:
                return TextureFormat::Bgra8Srgb;
            default:
                return TextureFormat::Unspecified;
            }
        }

        VkFormat EngineToVulkan_(TextureFormat format)
        {
            switch (format)
            {
            case TextureFormat::R8UnsignedNormalized:
                return VK_FORMAT_R8_UNORM;
            case TextureFormat::Rg8UnsignedNormalized:
                return VK_FORMAT_R8G8_UNORM;
            case TextureFormat::Rgb8UnsignedNormalized:
                return VK_FORMAT_R8G8B8_UNORM;
            case TextureFormat::Rgba8UnsignedNormalized:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::Bgr8UnsignedNormalized:
                return VK_FORMAT_B8G8R8_UNORM;
            case TextureFormat::Bgra8UnsignedNormalized:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::R8SignedNormalized:
                return VK_FORMAT_R8_SNORM;
            case TextureFormat::Rg8SignedNormalized:
                return VK_FORMAT_R8G8_SNORM;
            case TextureFormat::Rgb8SignedNormalized:
                return VK_FORMAT_R8G8B8_SNORM;
            case TextureFormat::Rgba8SignedNormalized:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case TextureFormat::Bgr8SignedNormalized:
                return VK_FORMAT_B8G8R8_SNORM;
            case TextureFormat::Bgra8SignedNormalized:
                return VK_FORMAT_B8G8R8A8_SNORM;
            case TextureFormat::R8UnsignedInteger:
                return VK_FORMAT_R8_UINT;
            case TextureFormat::Rg8UnsignedInteger:
                return VK_FORMAT_R8G8_UINT;
            case TextureFormat::Rgb8UnsignedInteger:
                return VK_FORMAT_R8G8B8_UINT;
            case TextureFormat::Rgba8UnsignedInteger:
                return VK_FORMAT_R8G8B8A8_UINT;
            case TextureFormat::Bgr8UnsignedInteger:
                return VK_FORMAT_B8G8R8_UINT;
            case TextureFormat::Bgra8UnsignedInteger:
                return VK_FORMAT_B8G8R8A8_UINT;
            case TextureFormat::R8SignedInteger:
                return VK_FORMAT_R8_SINT;
            case TextureFormat::Rg8SignedInteger:
                return VK_FORMAT_R8G8_SINT;
            case TextureFormat::Rgb8SignedInteger:
                return VK_FORMAT_R8G8B8_SINT;
            case TextureFormat::Rgba8SignedInteger:
                return VK_FORMAT_R8G8B8A8_SINT;
            case TextureFormat::Bgr8SignedInteger:
                return VK_FORMAT_B8G8R8_SINT;
            case TextureFormat::Bgra8SignedInteger:
                return VK_FORMAT_B8G8R8A8_SINT;
            case TextureFormat::R8Srgb:
                return VK_FORMAT_R8_SRGB;
            case TextureFormat::Rg8Srgb:
                return VK_FORMAT_R8G8_SRGB;
            case TextureFormat::Rgb8Srgb:
                return VK_FORMAT_R8G8B8_SRGB;
            case TextureFormat::Rgba8Srgb:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::Bgr8Srgb:
                return VK_FORMAT_B8G8R8_SRGB;
            case TextureFormat::Bgra8Srgb:
                return VK_FORMAT_B8G8R8A8_SRGB;
            default:
                return VK_FORMAT_UNDEFINED;
            }
        }

        VkImageViewType EngineToVulkan_(TextureViewDimension dimension)
        {
            switch (dimension)
            {
            case TextureViewDimension::Texture1D:
                return VK_IMAGE_VIEW_TYPE_1D;
            case TextureViewDimension::Texture1DArray:
                return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            case TextureViewDimension::Texture2D:
                return VK_IMAGE_VIEW_TYPE_2D;
            case TextureViewDimension::Texture2DArray:
                return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case TextureViewDimension::Texture3D:
                return VK_IMAGE_VIEW_TYPE_3D;
            }

            KITSUNE_UNREACHABLE();
        }

        VkImageLayout EngineToVulkan_(TextureUsage textureUsage)
        {
            switch (textureUsage)
            {
            case TextureUsage::Undefined:
                return { /* ... */ };
            case TextureUsage::RenderAttachment:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case TextureUsage::Presentation:
                return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
        }

        VkImage GetVulkanHandle_(const SharedPtr<Texture>& texture)
        {
            return DynamicPointerCast<VulkanTexture>(texture)->GetVulkanImage();
        }

        VkImageView GetVulkanHandle_(const SharedPtr<TextureView>& texture)
        {
            return DynamicPointerCast<VulkanTextureView>(texture)->GetVulkanImageView();
        }
    }

    VulkanTexture::VulkanTexture(VkImage image)
        : m_Image(image)
    {
    }

    VulkanTexture::~VulkanTexture()
    {
        if (m_Device != VK_NULL_HANDLE)
            ::vkDestroyImage(m_Device, m_Image, nullptr);

        /* Else, the image's lifetime is managed externally. */
    }

    VulkanTextureView::VulkanTextureView(
        VulkanGpuDevice& device,
        const SharedPtr<VulkanTexture>& texture,
        const TextureViewSpecifications& specifications)
        : m_Device(device), m_Texture(texture)
    {
        VkComponentMapping componentMapping = {
            VK_COMPONENT_SWIZZLE_R,
            VK_COMPONENT_SWIZZLE_G,
            VK_COMPONENT_SWIZZLE_B,
            VK_COMPONENT_SWIZZLE_A,
        };

        VkImageSubresourceRange subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        };

        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = m_Texture->GetVulkanImage(),
            .viewType = Details::EngineToVulkan_(specifications.Dimension),
            .format = Details::EngineToVulkan_(specifications.Format),
            .components = componentMapping,
            .subresourceRange = subresourceRange
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateImageView(m_Device.GetVulkanDevice(), &createInfo, nullptr,
                                &m_ImageView),
            "Failed to create a Vulkan image view.");
    }

    VulkanTextureView::~VulkanTextureView()
    {
        ::vkDestroyImageView(m_Device.GetVulkanDevice(), m_ImageView, nullptr);
    }
}
