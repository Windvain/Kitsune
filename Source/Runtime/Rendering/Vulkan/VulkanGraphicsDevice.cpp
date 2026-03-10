#include "Rendering/Vulkan/VulkanGraphicsDevice.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Logging/GlobalLog.h"

namespace Kitsune
{
    VkInstance VulkanGraphicsDevice::s_VulkanInstance;
    Usize VulkanGraphicsDevice::s_ReferenceCount = 0;

    VkDebugUtilsMessengerEXT VulkanGraphicsDevice::s_DebugMessenger = VK_NULL_HANDLE;

    static LogSeverity ToLoggingSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT severity)
    {
        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            return LogSeverity::Error;
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            return LogSeverity::Warning;
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            return LogSeverity::Info;

        return LogSeverity::Trace;
    }

    VulkanGraphicsDevice::VulkanGraphicsDevice(
        const StringView appName,
        const VulkanGpuRequirements& requirements)
    {
        Array<const char*> requestedExtensions = GetRequestedExtensions_();
        Array<const char*> requestedLayers = GetRequestedLayers_();

        if (s_ReferenceCount == 0)
            CreateInstance_(appName, requestedExtensions, requestedLayers);

        VkPhysicalDevice physicalDevice = PickSuitablePhysicalDevice_(requirements);
        (void)physicalDevice;       // TEMP: Shush!

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
            KITSUNE_VK_THROW_IF_FAIL(
                ::vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr),
                "Failed to enumerate over the list of supported extensions.");

            Array<VkExtensionProperties> extensionProperties(extensionCount,
                                                             VkExtensionProperties());

            KITSUNE_VK_THROW_IF_FAIL(
                ::vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                         extensionProperties.Data()),
                "Failed to enumerate over the list of supported extensions.");

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
                throw SystemException("Implementation does not support one or more of the "
                                      "requested extensions.");
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

                return { /* ... */ };
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

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateInstance(&instanceCreateInfo, nullptr, &s_VulkanInstance),
            "Failed to create a Vulkan instance.");

#if !defined(KITSUNE_BUILD_PRODUCTION)
        RegisterDebugCallback_();
#endif

        KITSUNE_ENGINE_INFO_FORMAT_("Successfully created the Vulkan instance \"{0}\".",
                                    appName);
    }

    void VulkanGraphicsDevice::DestroyInstance_()
    {
        KITSUNE_ASSERT(
            s_VulkanInstance != VK_NULL_HANDLE,
            "Attempted to destroy a VkInstance which had not been initialized or has already been "
            "destroyed.");

#if !defined(KITSUNE_BUILD_PRODUCTION)
        if (s_DebugMessenger != VK_NULL_HANDLE)
            UnregisterDebugCallback_();
#endif

        ::vkDestroyInstance(s_VulkanInstance, nullptr);
        KITSUNE_ENGINE_INFO_("Destroyed the Vulkan instance.");
    }

    void VulkanGraphicsDevice::RegisterDebugCallback_()
    {
        VkDebugUtilsMessengerCreateInfoEXT dbgCallbackCreateInfo = { /* ... */ };
        dbgCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        dbgCallbackCreateInfo.pfnUserCallback = &VulkanGraphicsDevice::DebugCallback_;
        dbgCallbackCreateInfo.pUserData = nullptr;

        dbgCallbackCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        dbgCallbackCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        auto createMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            ::vkGetInstanceProcAddr(s_VulkanInstance, "vkCreateDebugUtilsMessengerEXT"));

        if (createMessenger == nullptr)
        {
            KITSUNE_ENGINE_ERROR_("Failed to load vkCreateDebugUtilsMessengerEXT. The engine will "
                                  "not register a debug callback for Vulkan.");

            return;
        }

        KITSUNE_VK_THROW_IF_FAIL(
            createMessenger(s_VulkanInstance, &dbgCallbackCreateInfo, nullptr,
                            &s_DebugMessenger),
            "Failed to create the Vulkan debug callback. This is probably "
            "due to the system being out of memory.");

        KITSUNE_ENGINE_INFO_("Registered the Vulkan debug callback.");
    }

    void VulkanGraphicsDevice::UnregisterDebugCallback_()
    {
        auto destroyMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            ::vkGetInstanceProcAddr(s_VulkanInstance, "vkDestroyDebugUtilsMessengerEXT"));

        if (destroyMessenger == nullptr)
        {
            KITSUNE_ENGINE_ERROR_("Could not load vkDestroyDebugUtilsMessengerEXT(). The engine "
                                  "will not destroy the debug messenger.");

            return;
        }

        destroyMessenger(s_VulkanInstance, s_DebugMessenger, nullptr);
        KITSUNE_ENGINE_INFO_("Destroyed the Vulkan debug messenger.");
    }

    VkBool32 VulkanGraphicsDevice::DebugCallback_(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                  VkDebugUtilsMessageTypeFlagsEXT type,
                                                  const VkDebugUtilsMessengerCallbackDataEXT* data,
                                                  void* userData)
    {
        KITSUNE_UNUSED(type);
        KITSUNE_UNUSED(userData);

        Log("Kitsune", ToLoggingSeverity(severity), SourceLocation(), data->pMessage);
        return VK_FALSE;
    }

    VkPhysicalDevice VulkanGraphicsDevice::PickSuitablePhysicalDevice_(
        const VulkanGpuRequirements& requirements)
    {
        std::uint32_t deviceCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(s_VulkanInstance, &deviceCount, nullptr),
            "Failed to enumerate through the GPU(s) on the system.");

        Array<VkPhysicalDevice> physicalDevices(deviceCount, VkPhysicalDevice());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(s_VulkanInstance, &deviceCount, physicalDevices.Data()),
            "Failed to enumerate through the GPU(s) on the system.");

        if (physicalDevices.IsEmpty())
        {
            throw SystemException("Could not find any physical devices which support Vulkan. "
                                  "Either upgrade to a GPU which supports Vulkan, or use a "
                                  "different rendering backend.");
        }

        auto iter = Algorithms::FindIf(
            physicalDevices.GetBegin(), physicalDevices.GetEnd(),
            [&](VkPhysicalDevice physicalDevice) -> bool
            {
                return VulkanGraphicsDevice::IsPhysicalDeviceSuitable_(
                    physicalDevice,
                    requirements);
            });

        if (iter == physicalDevices.GetEnd())
        {
            throw SystemException("Could not find a physical device which suits the specified "
                                  "description.");
        }

        return *iter;
    }

    bool VulkanGraphicsDevice::IsPhysicalDeviceSuitable_(
        VkPhysicalDevice physicalDevice,
        const VulkanGpuRequirements& requirements)
    {
#pragma region GeneralRequirements
        VkPhysicalDeviceProperties properties;
        ::vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        KITSUNE_ENGINE_INFO_FORMAT_("Checking physical device \"{0}\"", properties.deviceName);

        if ((requirements.Vendor != 0) && (requirements.Vendor != properties.vendorID))
        {
            KITSUNE_ENGINE_ERROR_FORMAT_(
                "\tVendor PCI ID: {0:#x}, requested {1:#x} - FAIL",
                properties.vendorID, requirements.Vendor);

            return false;
        }

        KITSUNE_ENGINE_INFO_FORMAT_("\tVendor PCI ID: {0:#x} - OK", properties.vendorID);
#pragma endregion

#pragma region MemoryRequirements
        VkPhysicalDeviceMemoryProperties memoryProperties;
        ::vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        float gpuMemoryGiB = 0;
        for (std::uint32_t i = 0; i < memoryProperties.memoryHeapCount; ++i)
            gpuMemoryGiB += float(memoryProperties.memoryHeaps[i].size) / 1'073'741'824;

        if (gpuMemoryGiB < requirements.Memory)
        {
            KITSUNE_ENGINE_ERROR_FORMAT_("\tTotal Memory: {0} GiB, requested {1} GiB - FAIL",
                                         gpuMemoryGiB, requirements.Memory);

            return false;
        }

        KITSUNE_ENGINE_INFO_FORMAT_("\tTotal Memory: {0} GiB - OK", gpuMemoryGiB);
#pragma endregion

        KITSUNE_ENGINE_INFO_FORMAT_("\"{0}\" meets all GPU requirements.", properties.deviceName);
        return true;
    }
}
