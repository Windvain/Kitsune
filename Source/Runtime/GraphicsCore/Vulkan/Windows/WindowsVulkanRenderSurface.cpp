#include "GraphicsCore/Vulkan/VulkanRenderSurface.h"
#include "GraphicsCore/Vulkan/VulkanGpuInstance.h"

#include "Application/Windows/WindowsWindow.h"

namespace Kitsune
{
    void VulkanRenderSurface::CreatePlatformVulkanSurface_(Window* window)
    {
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
    }
}
