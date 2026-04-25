#pragma once

#include "GraphicsCore/Texture.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

#include "Foundation/Memory/SharedPtr.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]] TextureFormat VulkanToEngine_(VkFormat format);
        [[nodiscard]] VkFormat EngineToVulkan_(TextureFormat format);

        [[nodiscard]] VkImageViewType EngineToVulkan_(TextureViewDimension dimension);

        [[nodiscard]] VkImageLayout EngineToVulkan_(TextureUsage textureUsage);

        [[nodiscard]]
        VkImage GetVulkanHandle_(const SharedPtr<Texture>& texture);

        [[nodiscard]]
        VkImageView GetVulkanHandle_(const SharedPtr<TextureView>& texture);
    }

    class VulkanGpuDevice;

    class VulkanTexture : public Texture
    {
    public:
        VulkanTexture(VkImage image);
        ~VulkanTexture() override;

    public:
        [[nodiscard]]
        inline VkImage GetVulkanImage() const
        {
            return m_Image;
        }

    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        VkImage m_Image = VK_NULL_HANDLE;
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
        inline VkImageView GetVulkanImageView() const
        {
            return m_ImageView;
        }

    private:
        VulkanGpuDevice& m_Device;

        SharedPtr<VulkanTexture> m_Texture;
        VkImageView m_ImageView = VK_NULL_HANDLE;
    };
}
