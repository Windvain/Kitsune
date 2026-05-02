#pragma once

#include "GraphicsCore/RenderSurface.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

namespace Kitsune
{
    class Window;

    class VulkanGpuInstance;
    class VulkanRenderSurface;

    namespace Details
    {
        [[nodiscard]]
        SharedPtr<VulkanRenderSurface> ToImplementation_(
            const SharedPtr<RenderSurface>& surface);
    }

    class VulkanRenderSurface : public RenderSurface
    {
    public:
        VulkanRenderSurface(VulkanGpuInstance& instance, Window* window);
        ~VulkanRenderSurface() override;

    public:
        [[nodiscard]]
        RenderSurfaceCapabilities GetCapabilities(
            const SharedPtr<GpuDevice>& device) const override;

    public:
        [[nodiscard]]
        inline VkSurfaceKHR GetVulkanSurface() const
        {
            return m_Surface;
        }

        [[nodiscard]]
        Array<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormats(
            VkPhysicalDevice physicalDevice) const;

        [[nodiscard]]
        Array<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModes(
            VkPhysicalDevice physicalDevice) const;

    private:
        VulkanGpuInstance& m_Instance;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    };
}
