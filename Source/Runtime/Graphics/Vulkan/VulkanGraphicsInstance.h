#pragma once

#include "Graphics/GraphicsInstance.h"
#include "Graphics/Vulkan/VulkanHeader.h"       // IWYU pragma: keep

#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    class VulkanGraphicsInstance : public GraphicsInstance
    {
    public:
        using CreateDebugMessengerFunc = PFN_vkCreateDebugUtilsMessengerEXT;
        using DestroyDebugMessengerFunc = PFN_vkDestroyDebugUtilsMessengerEXT;

    public:
        // NOTE: Do not change `const String&` -> StringView. This argument needs to be
        // a null-terminated string.
        KITSUNE_API VulkanGraphicsInstance(bool enableDebug, const String& debugName);
        KITSUNE_API ~VulkanGraphicsInstance() override;

    private:
        [[nodiscard]] static Array<const char*> GetExtensions(bool enableDebug);
        [[nodiscard]] static Array<const char*> GetLayers(bool enableDebug);

        static void VerifyExtensionSupport(const Array<const char*>& extensions);
        static void VerifyLayerSupport(const Array<const char*>& layers);

    private:
        void RegisterDebugCallback();
        void UnregisterDebugCallback();

        bool InitializeDebugMessengerFunctions();
        static VkBool32 DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT type,
            const VkDebugUtilsMessengerCallbackDataEXT* data,
            void* userData);

    private:
        static constexpr Uint32 s_APIVersion = VK_API_VERSION_1_4;

    private:
        VkInstance m_Instance;

        CreateDebugMessengerFunc m_CreateDebugMessenger = nullptr;
        DestroyDebugMessengerFunc m_DestroyDebugMessenger = nullptr;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    };
}
