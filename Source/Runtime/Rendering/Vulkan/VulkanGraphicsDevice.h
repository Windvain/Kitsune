#pragma once

#include "Foundation/Containers/Array.h"
#include "Foundation/String/StringView.h"

#include "Rendering/GraphicsDevice.h"
#include "Rendering/Vulkan/VulkanHeader.h"

namespace Kitsune
{
    class VulkanGraphicsDevice : public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(const StringView appName);
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
        // There should only be ONE Vulkan instance for the entire app.
        static VkInstance s_VulkanInstance;
        static Usize s_ReferenceCount;

        static VkDebugUtilsMessengerEXT s_DebugMessenger;
    };
}
