#pragma once

#include "Foundation/Memory/SharedPtr.h"

#include "GraphicsCore/Texture.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class VulkanTexture;
    class VulkanTextureView;

    class VulkanGpuDevice;

    namespace Details
    {
        [[nodiscard]] TextureFormat ToTextureFormat_(VkFormat format);
        [[nodiscard]] VkFormat ToVkFormat_(TextureFormat format);

        [[nodiscard]]
        SharedPtr<VulkanTexture> ToImplementation_(const SharedPtr<Texture>& texture);

        [[nodiscard]]
        SharedPtr<VulkanTextureView> ToImplementation_(
            const SharedPtr<TextureView>& textureView);

        [[nodiscard]]
        VkImageViewType ToVkImageViewType_(TextureViewType type);

        [[nodiscard]]
        VkComponentMapping ToVkComponentMapping_(TextureViewComponentMapping mapping);

        [[nodiscard]]
        VkImageLayout ToVkImageLayout_(TextureLayout layout);
    }

    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(VkImage image, const Vector2<Uint32>& size);
        ~VulkanTexture() override;

    public:
        [[nodiscard]]
        inline Vector2<Uint32> GetSize() const override
        {
            return m_Size;
        }

    public:
        [[nodiscard]]
        inline VkImage GetVulkanImage() const
        {
            return m_Image;
        }

    private:
        VulkanGpuDevice* m_Device = nullptr;
        VkImage m_Image;

        Vector2<Uint32> m_Size;
    };

    class VulkanTextureView : public TextureView
    {
    public:
        VulkanTextureView(
            VulkanGpuDevice& device,
            const SharedPtr<VulkanTexture>& texture,
            const TextureViewSpecifications& specifications);

        ~VulkanTextureView() override;

    public:
        [[nodiscard]]
        inline Texture* GetTexture() const override
        {
            return m_Texture;
        }

    public:
        [[nodiscard]]
        inline VkImageView GetVulkanImageView() const
        {
            return m_ImageView;
        }

    private:
        VulkanGpuDevice& m_Device;

        VulkanTexture* m_Texture;       // Don't keep a SharedPtr.
        VkImageView m_ImageView = VK_NULL_HANDLE;
    };
}
