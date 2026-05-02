#include "GraphicsCore/Vulkan/VulkanTexture.h"
#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"

#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    namespace Details
    {
        TextureFormat ToTextureFormat_(VkFormat format)
        {
            switch (format)
            {
            case VK_FORMAT_R8_UNORM:
                return TextureFormat::R8Unorm;
            case VK_FORMAT_R8G8_UNORM:
                return TextureFormat::R8G8Unorm;
            case VK_FORMAT_R8G8B8A8_UNORM:
                return TextureFormat::R8G8B8A8Unorm;
            case VK_FORMAT_B8G8R8_UNORM:
                return TextureFormat::B8G8R8Unorm;
            case VK_FORMAT_B8G8R8A8_UNORM:
                return TextureFormat::B8G8R8A8Unorm;
            case VK_FORMAT_R8_SNORM:
                return TextureFormat::R8Snorm;
            case VK_FORMAT_R8G8_SNORM:
                return TextureFormat::R8G8Snorm;
            case VK_FORMAT_R8G8B8A8_SNORM:
                return TextureFormat::R8G8B8A8Snorm;
            case VK_FORMAT_B8G8R8_SNORM:
                return TextureFormat::B8G8R8Snorm;
            case VK_FORMAT_B8G8R8A8_SNORM:
                return TextureFormat::B8G8R8A8Snorm;
            case VK_FORMAT_R8_UINT:
                return TextureFormat::R8Uint;
            case VK_FORMAT_R8G8_UINT:
                return TextureFormat::R8G8Uint;
            case VK_FORMAT_R8G8B8A8_UINT:
                return TextureFormat::R8G8B8A8Uint;
            case VK_FORMAT_B8G8R8_UINT:
                return TextureFormat::B8G8R8Uint;
            case VK_FORMAT_B8G8R8A8_UINT:
                return TextureFormat::B8G8R8A8Uint;
            case VK_FORMAT_R8_SINT:
                return TextureFormat::R8Sint;
            case VK_FORMAT_R8G8_SINT:
                return TextureFormat::R8G8Sint;
            case VK_FORMAT_R8G8B8A8_SINT:
                return TextureFormat::R8G8B8A8Sint;
            case VK_FORMAT_B8G8R8_SINT:
                return TextureFormat::B8G8R8Sint;
            case VK_FORMAT_B8G8R8A8_SINT:
                return TextureFormat::B8G8R8A8Sint;
            case VK_FORMAT_R8_SRGB:
                return TextureFormat::R8Srgb;
            case VK_FORMAT_R8G8_SRGB:
                return TextureFormat::R8G8Srgb;
            case VK_FORMAT_R8G8B8A8_SRGB:
                return TextureFormat::R8G8B8A8Srgb;
            case VK_FORMAT_B8G8R8_SRGB:
                return TextureFormat::B8G8R8Srgb;
            case VK_FORMAT_B8G8R8A8_SRGB:
                return TextureFormat::B8G8R8A8Srgb;
            default:
                return TextureFormat::Unknown;
            }
        }

        VkFormat ToVkFormat_(TextureFormat format)
        {
            switch (format)
            {
            case TextureFormat::R8Unorm:
                return VK_FORMAT_R8_UNORM;
            case TextureFormat::R8G8Unorm:
                return VK_FORMAT_R8G8_UNORM;
            case TextureFormat::R8G8B8A8Unorm:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::B8G8R8Unorm:
                return VK_FORMAT_B8G8R8_UNORM;
            case TextureFormat::B8G8R8A8Unorm:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::R8Snorm:
                return VK_FORMAT_R8_SNORM;
            case TextureFormat::R8G8Snorm:
                return VK_FORMAT_R8G8_SNORM;
            case TextureFormat::R8G8B8A8Snorm:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case TextureFormat::B8G8R8Snorm:
                return VK_FORMAT_B8G8R8_SNORM;
            case TextureFormat::B8G8R8A8Snorm:
                return VK_FORMAT_B8G8R8A8_SNORM;
            case TextureFormat::R8Uint:
                return VK_FORMAT_R8_UINT;
            case TextureFormat::R8G8Uint:
                return VK_FORMAT_R8G8_UINT;
            case TextureFormat::R8G8B8A8Uint:
                return VK_FORMAT_R8G8B8A8_UINT;
            case TextureFormat::B8G8R8Uint:
                return VK_FORMAT_B8G8R8_UINT;
            case TextureFormat::B8G8R8A8Uint:
                return VK_FORMAT_B8G8R8A8_UINT;
            case TextureFormat::R8Sint:
                return VK_FORMAT_R8_SINT;
            case TextureFormat::R8G8Sint:
                return VK_FORMAT_R8G8_SINT;
            case TextureFormat::R8G8B8A8Sint:
                return VK_FORMAT_R8G8B8A8_SINT;
            case TextureFormat::B8G8R8Sint:
                return VK_FORMAT_B8G8R8_SINT;
            case TextureFormat::B8G8R8A8Sint:
                return VK_FORMAT_B8G8R8A8_SINT;
            case TextureFormat::R8Srgb:
                return VK_FORMAT_R8_SRGB;
            case TextureFormat::R8G8Srgb:
                return VK_FORMAT_R8G8_SRGB;
            case TextureFormat::R8G8B8A8Srgb:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::B8G8R8Srgb:
                return VK_FORMAT_B8G8R8_SRGB;
            case TextureFormat::B8G8R8A8Srgb:
                return VK_FORMAT_B8G8R8A8_SRGB;
            default:
                return VK_FORMAT_UNDEFINED;
            }
        }

        SharedPtr<VulkanTexture> ToImplementation_(const SharedPtr<Texture>& texture)
        {
            return DynamicPointerCast<VulkanTexture>(texture);
        }

        SharedPtr<VulkanTextureView> ToImplementation_(
            const SharedPtr<TextureView>& textureView)
        {
            return DynamicPointerCast<VulkanTextureView>(textureView);
        }


        VkImageViewType ToVkImageViewType_(TextureViewType type)
        {
            switch (type)
            {
            case TextureViewType::Texture1D:
                return VK_IMAGE_VIEW_TYPE_1D;
            case TextureViewType::Texture1DArray:
                return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
            case TextureViewType::Texture2D:
                return VK_IMAGE_VIEW_TYPE_2D;
            case TextureViewType::Texture2DArray:
                return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
            case TextureViewType::Texture3D:
                return VK_IMAGE_VIEW_TYPE_3D;
            }

            KITSUNE_UNREACHABLE();
        }

        static VkComponentSwizzle ToVkComponentSwizzle_(TextureComponentSwizzle swizzle)
        {
            switch (swizzle)
            {
            case TextureComponentSwizzle::Zero:
                return VK_COMPONENT_SWIZZLE_ZERO;
            case TextureComponentSwizzle::One:
                return VK_COMPONENT_SWIZZLE_ONE;
            case TextureComponentSwizzle::Red:
                return VK_COMPONENT_SWIZZLE_R;
            case TextureComponentSwizzle::Green:
                return VK_COMPONENT_SWIZZLE_G;
            case TextureComponentSwizzle::Blue:
                return VK_COMPONENT_SWIZZLE_B;
            case TextureComponentSwizzle::Alpha:
                return VK_COMPONENT_SWIZZLE_A;
            }

            KITSUNE_UNREACHABLE();
        }

        VkComponentMapping ToVkComponentMapping_(TextureViewComponentMapping mapping)
        {
            return {
                .r = ToVkComponentSwizzle_(mapping.Red),
                .g = ToVkComponentSwizzle_(mapping.Green),
                .b = ToVkComponentSwizzle_(mapping.Blue),
                .a = ToVkComponentSwizzle_(mapping.Alpha),
            };
        }

        VkImageLayout ToVkImageLayout_(TextureLayout layout)
        {
            switch (layout)
            {
            case TextureLayout::Undefined:
                return { /* ... */ };
            case TextureLayout::RenderTarget:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case TextureLayout::Presentation:
                return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            }
        }
    }

    VulkanTexture::VulkanTexture(VkImage image, const Vector2<Uint32>& size)
        : m_Image(image), m_Size(size)
    {
    }

    VulkanTexture::~VulkanTexture()
    {
        if (m_Device != nullptr)
            (void)0;        // Implemented later, destroy the image.

        /* Else, the image is managed externally. */
    }

    VulkanTextureView::VulkanTextureView(
        VulkanGpuDevice& device,
        const SharedPtr<VulkanTexture>& texture,
        const TextureViewSpecifications& specifications)
        : m_Device(device), m_Texture(texture.Get())
    {
        if (texture == nullptr)
        {
            throw InvalidArgumentException(
                "Failed to construct a Vulkan texture view. The texture is NULL.");
        }

        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = m_Texture->GetVulkanImage(),
            .viewType = Details::ToVkImageViewType_(specifications.Type),
            .format = Details::ToVkFormat_(specifications.Format),
            .components = Details::ToVkComponentMapping_(specifications.Mapping),
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateImageView(
                m_Device.GetVulkanDevice(), &createInfo, nullptr, &m_ImageView),
            "Failed to create a Vulkan image view.");
    }

    VulkanTextureView::~VulkanTextureView()
    {
        ::vkDestroyImageView(m_Device.GetVulkanDevice(), m_ImageView, nullptr);
    }
}
