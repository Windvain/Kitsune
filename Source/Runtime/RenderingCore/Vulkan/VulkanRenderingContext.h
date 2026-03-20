#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "RenderingCore/RenderingContext.h"
#include "RenderingCore/RenderingDevice.h"

#include "RenderingCore/Vulkan/VulkanHeader.h"
#include "RenderingCore/Vulkan/VulkanRenderingDevice.h"

namespace Kitsune
{
    class VulkanRenderingContext : public RenderingContext
    {
    public:
        VulkanRenderingContext(const StringView applicationName);
        ~VulkanRenderingContext();

    public:
        Array<RenderingDeviceInformation> GetAvailableDevicesInformation() const override;

        RenderingDevice* CreateRenderingDevice(Uint32 deviceIndex) override;
        void DestroyRenderingDevice(RenderingDevice* device) override;

    private:
        static Array<const char*> GetRequestedExtensions_();
        static Array<const char*> GetRequestedLayers_();

        // Throws if the specified extensions/layers is not supported.
        static void VerifyExtensions_(const Array<const char*>& extensions);
        static void VerifyLayers_(const Array<const char*>& layers);

    private:
        void RegisterDebugCallback_();
        void UnregisterDebugCallback_();

        void RetrieveDebugFunctions_();
        static VkBool32 DebugCallback_(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                       VkDebugUtilsMessageTypeFlagsEXT type,
                                       const VkDebugUtilsMessengerCallbackDataEXT* data,
                                       void* userData);

    private:
        Array<const char*> m_Extensions;
        Array<const char*> m_Layers;

        String m_ApplicationName;
        VkInstance m_Instance = VK_NULL_HANDLE;

        PFN_vkCreateDebugUtilsMessengerEXT m_CreateDebugMessenger;
        PFN_vkDestroyDebugUtilsMessengerEXT m_DestroyDebugMessenger;
        VkDebugUtilsMessengerEXT m_DebugMessenger;

        Array<VkPhysicalDevice> m_PhysicalDevices;
        Array<RenderingDeviceInformation> m_PhysicalDevicesInfo;

        Array<VulkanRenderingDevice*> m_RenderingDevices;
    };
}
