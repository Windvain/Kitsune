#include "Rendering/Vulkan/VulkanGraphicsDevice.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Logging/GlobalLog.h"

namespace Kitsune
{
    VkInstance VulkanGraphicsDevice::s_VulkanInstance;
    Usize VulkanGraphicsDevice::s_ReferenceCount = 0;

    VulkanGraphicsDevice::VulkanGraphicsDevice(const StringView appName)
    {
        Uint32 vulkanVersion;
        if (::vkEnumerateInstanceVersion(&vulkanVersion) != VK_SUCCESS)
        {
            throw SystemException(
                "Failed to retrieve the instance-level version supported by the "
                "implementation. This shouldn't happen..?");
        }

        Array<const char*> requestedExtensions = GetRequestedExtensions_();
        Array<const char*> requestedLayers = GetRequestedLayers_();

        if (s_ReferenceCount == 0)
            CreateInstance_(appName, requestedExtensions, requestedLayers);

        ++s_ReferenceCount;
    }

    VulkanGraphicsDevice::~VulkanGraphicsDevice()
    {
        --s_ReferenceCount;

        if (s_ReferenceCount == 0)
            DestroyInstance_();
    }

    Array<const char*> VulkanGraphicsDevice::GetRequestedExtensions_()
    {
        Array<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#if !defined(KITSUNE_BUILD_PRODUCTION)
        extensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#if defined(KITSUNE_OS_WINDOWS)
        extensions.PushBack(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#pragma region ExtensionValidation
        KITSUNE_ENGINE_INFO_FORMAT_(
            "Starting Vulkan extension validations, extension count: {0}",
            extensions.Size());

        {
            std::uint32_t extensionCount;
            VkResult result;

            result = ::vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            if (result != VK_SUCCESS)
            {
                throw SystemException(
                    "Failed to enumerate over the list of supported extensions.");
            }

            Array<VkExtensionProperties> extensionProperties(extensionCount, VkExtensionProperties());
            result = ::vkEnumerateInstanceExtensionProperties(
                nullptr, &extensionCount, extensionProperties.Data());

            if (result != VK_SUCCESS)
            {
                throw SystemException(
                    "Failed to enumerate over the list of supported extensions.");
            }

            bool hasUnsupportedExtensions = false;
            for (const char* extensionName : extensions)
            {
                Uint32 version;
                auto iter = Algorithms::FindIf(
                    extensionProperties.GetBegin(), extensionProperties.GetEnd(),
                    [&](const VkExtensionProperties& properties) -> bool
                    {
                        if (StringView(properties.extensionName) == extensionName)
                        {
                            version = properties.specVersion;
                            return true;
                        }

                        return false;
                    });

                if (iter == extensionProperties.GetEnd())
                {
                    KITSUNE_ENGINE_ERROR_FORMAT_("\tExtension {0} NOT found.", extensionName);
                    hasUnsupportedExtensions = true;
                }
                else
                {
                    KITSUNE_ENGINE_INFO_FORMAT_(
                        "\tFound extension {0} - {1}.{2}.{3}",
                        extensionName,
                        VK_API_VERSION_MAJOR(version),
                        VK_API_VERSION_MINOR(version),
                        VK_API_VERSION_PATCH(version));
                }
            }

            if (hasUnsupportedExtensions)
            {
                KITSUNE_ENGINE_ERROR_(
                    "The implementation does not support one or more of the engine's "
                    "requested extensions.");

                throw SystemException("Implementation does not support one or more of the "
                                      "requested extension.");
            }
        }

        KITSUNE_ENGINE_INFO_("Vulkan extension validation success.");
#pragma endregion

        return extensions;
    }

    Array<const char*> VulkanGraphicsDevice::GetRequestedLayers_()
    {
        Array<const char*> layers;

#if !defined(KITSUNE_BUILD_PRODUCTION)
        layers.PushBack("VK_LAYER_KHRONOS_validation");
#endif

#pragma region LayerValidation
        KITSUNE_ENGINE_INFO_FORMAT_(
            "Starting Vulkan layer validations, layer count: {0}",
            layers.Size());

        {
            std::uint32_t layerCount;
            VkResult result;

            result = ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            if (result != VK_SUCCESS)
            {
                KITSUNE_ENGINE_ERROR_("Failed to enumerate over the list of supported layers. "
                                      "Requesting 0 layers.");

                return { /* ... */ };
            }

            Array<VkLayerProperties> layerProperties(layerCount, VkLayerProperties());
            result = ::vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.Data());

            if (result != VK_SUCCESS)
            {
                KITSUNE_ENGINE_ERROR_("Failed to enumerate over the list of supported layers. "
                                      "The engine will not be requesting any layers.");

                return { /* ... */ };
            }

            bool hasUnsupportedLayers = false;
            for (const char* layerName : layers)
            {
                Uint32 version;
                auto iter = Algorithms::FindIf(
                    layerProperties.GetBegin(), layerProperties.GetEnd(),
                    [&](const VkLayerProperties& properties) -> bool
                    {
                        if (StringView(properties.layerName) == layerName)
                        {
                            version = properties.specVersion;
                            return true;
                        }

                        return false;
                    });

                if (iter == layerProperties.GetEnd())
                {
                    KITSUNE_ENGINE_ERROR_FORMAT_("\tLayer {0} NOT found.", layerName);
                    hasUnsupportedLayers = true;
                }
                else
                {
                    KITSUNE_ENGINE_INFO_FORMAT_(
                        "\tFound layer {0} - {1}.{2}.{3}",
                        layerName,
                        VK_API_VERSION_MAJOR(version),
                        VK_API_VERSION_MINOR(version),
                        VK_API_VERSION_PATCH(version));
                }
            }

            if (hasUnsupportedLayers)
            {
                KITSUNE_ENGINE_ERROR_(
                    "The implementation does not support one or more of the engine's "
                    "requested layers.");

                KITSUNE_ENGINE_ERROR_(
                    "Consider disabling the features that caused this "
                    "layer to be requested or updating your implementation to support "
                    "the missing layer(s).");

                throw SystemException("Implementation does not support one or more of the "
                                      "requested layers.");
            }
        }

        KITSUNE_ENGINE_INFO_("Vulkan layer validation success.");
#pragma endregion

        return layers;
    }

    void VulkanGraphicsDevice::CreateInstance_(const StringView appName,
                                               const Array<const char*>& extensions,
                                               const Array<const char*>& layers)
    {
        VkApplicationInfo applicationInfo = { /* ... */ };
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = appName.Data();
        applicationInfo.pEngineName = "Kitsune Engine";
        applicationInfo.apiVersion = VK_API_VERSION_1_4;

        VkInstanceCreateInfo instanceCreateInfo = { /* ... */ };
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        instanceCreateInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.Size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.Data();

        instanceCreateInfo.enabledLayerCount = static_cast<std::uint32_t>(layers.Size());
        instanceCreateInfo.ppEnabledLayerNames = layers.Data();

        KITSUNE_ENGINE_INFO_FORMAT_("Creating the Vulkan instance \"{0}\".",
                                    appName);

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateInstance(&instanceCreateInfo, nullptr, &s_VulkanInstance),
            "Failed to create a Vulkan instance.");
    }

    void VulkanGraphicsDevice::DestroyInstance_()
    {
        ::vkDestroyInstance(s_VulkanInstance, nullptr);
    }
}
