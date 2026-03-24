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
        ~VulkanRenderingContext();

    public:
        RenderingDevice* CreateRenderingDevice(Uint32 deviceIndex,
                                               WindowHandle surface) override;

        void DestroyRenderingDevice(RenderingDevice* device) override;

    private:
        static Array<const char*> GetRequestedExtensions_();
        static Array<const char*> GetRequestedLayers_();

        // Throws if the specified extensions/layers is not supported.
        static void VerifyExtensionsSupport_(const Array<const char*>& extensions);
        static void VerifyLayersSupport_(const Array<const char*>& layers);

    private:
        void RegisterDebugCallback_();
        void UnregisterDebugCallback_();

        void RetrieveDebugFunctions_();
        static VkBool32 DebugCallback_(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                       VkDebugUtilsMessageTypeFlagsEXT type,
                                       const VkDebugUtilsMessengerCallbackDataEXT* data,
                                       void* userData);

    private:
        VkInstance m_Instance = VK_NULL_HANDLE;

        PFN_vkCreateDebugUtilsMessengerEXT m_CreateDebugMessenger;
        PFN_vkDestroyDebugUtilsMessengerEXT m_DestroyDebugMessenger;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        Array<VkPhysicalDevice> m_PhysicalDevices;
        Array<ScopedPtr<VulkanRenderingDevice>> m_RenderingDevices;
    };
}
