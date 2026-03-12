#pragma once

#include "Foundation/Containers/Array.h"
#include "Foundation/String/StringView.h"

#include "Rendering/GraphicsDevice.h"
#include "Rendering/Vulkan/VulkanHeader.h"

namespace Kitsune
{
    struct VulkanGpuRequirements
    {
        VkDeviceSize Memory = 0;
        std::uint32_t Vendor = 0;

        VkPhysicalDeviceFeatures Features = { /* ... */ };
    };

    class VulkanGraphicsDevice : public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(const StringView appName,
                             const VulkanGpuRequirements& requirements);

        ~VulkanGraphicsDevice();

    private:
        static Array<const char*> GetRequestedExtensions_();
        static Array<const char*> GetRequestedLayers_();

        static void CreateInstance_(const StringView appName,
                                    const Array<const char*>& extensions,
                                    const Array<const char*>& layers);

        static void DestroyInstance_();

        static void RegisterDebugCallback_();
        static void UnregisterDebugCallback_();

        static VkBool32 DebugCallback_(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                       VkDebugUtilsMessageTypeFlagsEXT type,
                                       const VkDebugUtilsMessengerCallbackDataEXT* data,
                                       void* userData);

    private:
        static VkPhysicalDevice PickSuitablePhysicalDevice_(
            const VulkanGpuRequirements& requirements);

        static bool IsPhysicalDeviceSuitable_(
            VkPhysicalDevice physicalDevice,
            const VulkanGpuRequirements& requirements);

    private:
        static Array<VkDeviceQueueCreateInfo> GetQueueCreateInfo_(
            VkPhysicalDevice physicalDevice);

    private:
        // There should only be ONE Vulkan instance for the entire app.
        static VkInstance s_VulkanInstance;
        static Usize s_ReferenceCount;

        static VkDebugUtilsMessengerEXT s_DebugMessenger;

    private:
        VkDevice m_Device = VK_NULL_HANDLE;
        Array<VkQueue> m_Queues;
    };
}
