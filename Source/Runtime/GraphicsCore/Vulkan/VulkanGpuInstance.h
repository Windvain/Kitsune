#pragma once

#include "GraphicsCore/GpuInstance.h"
#include "GraphicsCore/Vulkan/VulkanUtilities.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Logging/LogPayload.h"

namespace Kitsune
{
    namespace Details
    {
        [[nodiscard]]
        LogSeverity VulkanToEngine_(VkDebugUtilsMessageSeverityFlagsEXT severity);
    }

    class VulkanGpuDevice;
    class VulkanRenderSurface;

    class VulkanGpuInstance : public GpuInstance
    {
    public:
        VulkanGpuInstance(GpuInstanceDescription description);
        ~VulkanGpuInstance() override;

    public:
        [[nodiscard]]
        SharedPtr<GpuDevice> RequestDevice(
            const GpuDeviceRequirements& requirements) override;

        [[nodiscard]]
        SharedPtr<RenderSurface> RequestSurface(Window* window) override;

    public:
        [[nodiscard]]
        inline GpuInstanceDescription GetDescription() const override
        {
            return m_Description;
        }

    public:
        [[nodiscard]]
        inline VkInstance GetVulkanInstance() const
        {
            return m_Instance;
        }

    private:
        [[nodiscard]] static Array<const char*> GetExtensions_();
        [[nodiscard]] Array<const char*> GetLayers_() const;

        // Throws if one of the specified extensions/layers are not supported.
        static void VerifyExtensionSupport_(const Array<const char*>& extensions);
        static void VerifyLayerSupport_(const Array<const char*>& layers);

    private:
        bool RetrieveDebugFunctions_();

        void RegisterDebugCallback_();
        void UnregisterDebugCallback_();

        static VkBool32 DebugCallback_(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* data,
            void* userData);

    private:
        [[nodiscard]]
        static Array<VkDeviceQueueCreateInfo> GetQueueCreateInfos_(
            VkPhysicalDevice physicalDevice,
            const Array<CommandQueueSpecifications>& commandQueues,
            const SharedPtr<RenderSurface>& supportedSurface);

        [[nodiscard]]
        static bool QuerySurfaceSupport_(
            VkPhysicalDevice physicalDevice,
            Uint32 familyIndex,
            const SharedPtr<RenderSurface>& surface);

        [[nodiscard]]
        static Array<const char*> GetDeviceExtensions_();

        [[nodiscard]]
        static bool DeviceHasExtensionSupport_(
            VkPhysicalDevice device,
            const Array<const char*>& deviceExtensions);

        [[nodiscard]]
        static bool DeviceSupportsFeatures_(VkPhysicalDevice device);

    private:
        static constexpr Uint32 s_VulkanApiVersion = VK_API_VERSION_1_3;

    private:
        VkInstance m_Instance = VK_NULL_HANDLE;
        GpuInstanceDescription m_Description;

        Array<SharedPtr<VulkanGpuDevice>> m_Devices;
        Array<SharedPtr<VulkanRenderSurface>> m_Surfaces;

        using CreateDebugMessengerFunc = PFN_vkCreateDebugUtilsMessengerEXT;
        using DestroyDebugMessengerFunc = PFN_vkDestroyDebugUtilsMessengerEXT;

        CreateDebugMessengerFunc m_CreateDebugMessenger = nullptr;
        DestroyDebugMessengerFunc m_DestroyDebugMessenger = nullptr;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    };
}
