#include "RenderingCore/Vulkan/VulkanRenderingDevice.h"
#include "Application/Windows/WindowsWindow.h"

namespace Kitsune
{
    void VulkanRenderingDevice::InitializePlatformSurface_(WindowHandle windowHandle)
    {
        auto* windowsWindow = dynamic_cast<WindowsWindow*>(windowHandle);
        HWND hwnd = windowsWindow->GetNativeHandle();

        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { /* ... */ };
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hwnd = hwnd;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface),
            "Failed to create a Win32 surface from the newly created window.");
    }
}
