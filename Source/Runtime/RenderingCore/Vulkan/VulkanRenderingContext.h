#pragma once

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Containers/Array.h"

#include "RenderingCore/RenderingDevice.h"
#include "RenderingCore/RenderingContext.h"

#include "RenderingCore/Vulkan/VulkanHeader.h"

namespace Kitsune
{
    class VulkanRenderingDevice;

    class VulkanRenderingContext : public RenderingContext
    {
    public:
        VulkanRenderingContext();
        ~VulkanRenderingContext() override;

    public:
        [[nodiscard]]
        RenderingDevice* CreateRenderingDevice(
            Uint32 deviceIndex,
            WindowHandle windowHandle) override;

        void DestroyRenderingDevice(RenderingDevice* device) override;

    private:
        [[nodiscard]] static Array<const char*> GetRequestedExtensions_();
        [[nodiscard]] static Array<const char*> GetRequestedLayers_();

        // Throws if the specified extensions/layers is not supported.
        static void VerifyExtensionsSupport_(const Array<const char*>& extensions);
        static void VerifyLayersSupport_(const Array<const char*>& layers);

    private:
        void RegisterDebugCallback_();
        void UnregisterDebugCallback_();

        bool RetrieveDebugFunctions_();

        static VkBool32 DebugCallback_(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* data,
            void* userData);

    private:
        // These names are simply too long!
        using CreateDebugMessengerFunc = PFN_vkCreateDebugUtilsMessengerEXT;
        using DestroyDebugMessengerFunc = PFN_vkDestroyDebugUtilsMessengerEXT;

    private:
        VkInstance m_Instance = VK_NULL_HANDLE;

        CreateDebugMessengerFunc m_CreateDebugMessenger = nullptr;
        DestroyDebugMessengerFunc m_DestroyDebugMessenger = nullptr;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

        Array<VkPhysicalDevice> m_PhysicalDevices;
        Array<ScopedPtr<VulkanRenderingDevice>> m_RenderingDevices;
    };
}
