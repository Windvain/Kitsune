#include "Graphics/Vulkan/VulkanGraphicsInstance.h"
#include "Graphics/Vulkan/VulkanGraphicsDevice.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Algorithms/Contains.h"

namespace Kitsune
{
    [[nodiscard]]
    inline static LogSeverity ToEngineSeverity(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity)
    {
        if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            return LogSeverity::Error;
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            return LogSeverity::Warning;
        else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            return LogSeverity::Info;

        return LogSeverity::Trace;
    }

    [[nodiscard]]
    inline static String ToVendorString(Uint32 vendorID)
    {
        switch (vendorID)
        {
        case 0x10DE: return "NVidia";
        case 0x1022: return "AMD";
        case 0x8086: return "Intel";
        default:
            return "Unknown";
        }

        KITSUNE_UNREACHABLE();
    }

    [[nodiscard]]
    inline static GraphicsDeviceType ToGraphicsDeviceType(VkPhysicalDeviceType type)
    {
        switch (type)
        {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return GraphicsDeviceType::Discrete;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return GraphicsDeviceType::Integrated;

        case VK_PHYSICAL_DEVICE_TYPE_CPU:           [[fallthrough]];
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:   [[fallthrough]];
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:         [[fallthrough]];
        default:
            return GraphicsDeviceType::Unknown;
        }
    }

    // m_Instance is initialized by vkCreateInstance().
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    VulkanGraphicsInstance::VulkanGraphicsInstance(
        bool enableDebug, const String& debugName)
    {
        Array<const char*> extensions = GetExtensions(enableDebug);
        Array<const char*> layers = GetLayers(enableDebug);

        VerifyExtensionSupport(extensions);
        VerifyLayerSupport(layers);

        VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = debugName.Raw(),
            .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
            .pEngineName = "Kitsune Engine",
            .engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
            .apiVersion = s_APIVersion
        };

        VkInstanceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount =  static_cast<Uint32>(layers.Size()),
            .ppEnabledLayerNames = layers.Data(),
            .enabledExtensionCount = static_cast<Uint32>(extensions.Size()),
            .ppEnabledExtensionNames = extensions.Data()
        };

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateInstance(&createInfo, nullptr, &m_Instance),
            "Failed to create a Vulkan instance.");

        if (enableDebug)
            RegisterDebugCallback();

        InitializePhysicalDevices();
    }

    VulkanGraphicsInstance::~VulkanGraphicsInstance()
    {
        UnregisterDebugCallback();
        vkDestroyInstance(m_Instance, nullptr);
    }

    SharedPtr<GraphicsDevice> VulkanGraphicsInstance::RequestDevice()
    {
        return MakeShared<VulkanGraphicsDevice>(
            m_Devices[0],
            GetVulkanDeviceDescription(m_Devices[0]));
    }

    SharedPtr<GraphicsDevice> VulkanGraphicsInstance::RequestDevice(
        const GraphicsDeviceUUID& uuid)
    {
        auto iter = Algorithms::FindIf(
            m_DeviceDescriptions.GetBegin(), m_DeviceDescriptions.GetEnd(),
            [&](const GraphicsDeviceDescription& description)
            {
                return (description.UUID() == uuid);
            });

        if (iter == m_DeviceDescriptions.GetEnd())
            return nullptr;

        Index index = iter - m_DeviceDescriptions.GetBegin();
        return MakeShared<VulkanGraphicsDevice>(
            m_Devices[index],
            GetVulkanDeviceDescription(m_Devices[index]));
    }

    SharedPtr<GraphicsDevice> VulkanGraphicsInstance::RequestDevice(
        GraphicsDevicePreference preference)
    {
        // For now, just map GraphicsDeviceType::PowerSaving to dntegrated GPUs,
        // and GraphicsDeviceType::HighPerformance to discrete GPUs.
        GraphicsDeviceType requestedType =
            (preference == GraphicsDevicePreference::HighPerformance) ?
                GraphicsDeviceType::Discrete :
                GraphicsDeviceType::Integrated;

        auto iter = Algorithms::FindIf(
            m_DeviceDescriptions.GetBegin(), m_DeviceDescriptions.GetEnd(),
            [&](const GraphicsDeviceDescription& description)
            {
                return (description.Type() == requestedType);
            });

        if (iter == m_DeviceDescriptions.GetEnd())
            return RequestDevice();

        Index index = iter - m_DeviceDescriptions.GetBegin();
        return MakeShared<VulkanGraphicsDevice>(
            m_Devices[index],
            GetVulkanDeviceDescription(m_Devices[index]));
    }

    Array<const char*> VulkanGraphicsInstance::GetExtensions(bool enableDebug)
    {
        Array<const char*> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME           // For VkSurfaceKHR.
        };

        // For operating system specific implementations of VkSurfaceKHR.
#if defined(KITSUNE_OS_WINDOWS)
        extensions.PushBack(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

        // For VkDebugUtilsMessengerEXT and its creation/deletion functions.
        if (enableDebug)
            extensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return extensions;
    }

    Array<const char*> VulkanGraphicsInstance::GetLayers(bool enableDebug)
    {
        Array<const char*> layers;

        // Used in development: Make sure Vulkan usage aligns with the
        // Vulkan standards.
        if (enableDebug)
            layers.PushBack("VK_LAYER_KHRONOS_validation");

        return layers;
    }

    void VulkanGraphicsInstance::VerifyExtensionSupport(
        const Array<const char*>& extensions)
    {
        Uint32 extensionCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceExtensionProperties(
                nullptr, &extensionCount, nullptr),
            "Failed to enumerate over the list of supported extensions.");

        Array<VkExtensionProperties> propertiesArray(
            extensionCount, VkExtensionProperties());

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceExtensionProperties(
                nullptr, &extensionCount, propertiesArray.Data()),
            "Failed to enumerate over the list of supported extensions.");

        bool shouldThrow = false;
        for (const char* extension : extensions)
        {
            const auto predicate = [&](const VkExtensionProperties& properties) -> bool
            {
                return (StringView(properties.extensionName) == extension);
            };

            if (!Algorithms::ContainsIf(propertiesArray.GetBegin(),
                                        propertiesArray.GetEnd(), predicate))
            {
                KITSUNE_ENGINE_FATAL_FORMAT(
                    Graphics,
                    "The extension {0} which is required by the engine could not be "
                    "found.",
                    extension);

                shouldThrow = true;
            }
        }

        if (shouldThrow)
        {
            throw SystemException(
                "Failed to find one/multiple Vulkan instance extensions.");
        }
    }

    void VulkanGraphicsInstance::VerifyLayerSupport(const Array<const char*>& layers)
    {
        Uint32 layerCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr),
            "Failed to enumerate over the instance's layers.");

        Array<VkLayerProperties> propertiesArray(layerCount, VkLayerProperties());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceLayerProperties(&layerCount, propertiesArray.Data()),
            "Failed to enumerate over the instance's layers.");

        bool shouldThrow = false;
        for (const char* layer : layers)
        {
            const auto predicate = [&](const VkLayerProperties& properties) -> bool
            {
                return (StringView(properties.layerName) == layer);
            };

            if (!Algorithms::ContainsIf(propertiesArray.GetBegin(),
                                        propertiesArray.GetEnd(), predicate))
            {
                KITSUNE_ENGINE_ERROR_FORMAT(
                    Graphics,
                    "The layer {0} which is required by the engine could not be "
                    "found.",
                    layer);

                shouldThrow = true;
            }
        }

        if (shouldThrow)
        {
            throw SystemException(
                "Failed to find one/multiple Vulkan instance layers.");
        }
    }

    void VulkanGraphicsInstance::RegisterDebugCallback()
    {
        KITSUNE_ASSERT(
            m_Instance != VK_NULL_HANDLE,
            "RegisterDebugCallback() should be called after m_Instance has been "
            "initialized.");

        if (!InitializeDebugMessengerFunctions())
        {
            KITSUNE_ENGINE_WARN(
                Graphics,
                "Failed to retrieve function pointers to vkCreateDebugUtilsMessengerEXT "
                "and vkDestroyDebugUtilsMessengerEXT. The engine will not register any "
                "debug callbacks, Vulkan errors will not show up in the logs.");
        }

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = &VulkanGraphicsInstance::DebugCallback,
            .pUserData = nullptr
        };

        VkResult result = m_CreateDebugMessenger(
            m_Instance, &createInfo, nullptr, &m_DebugMessenger);

        if (result != VK_SUCCESS)
        {
            KITSUNE_ENGINE_WARN_FORMAT(
                Graphics,
                "Failed to create the Vulkan debug callback. "
                "vkCreateDebugUtilsMessengerEXT() returned a VkResult error {0}.",
                string_VkResult(result));
        }
    }

    void VulkanGraphicsInstance::UnregisterDebugCallback()
    {
        KITSUNE_ASSERT(
            m_Instance != VK_NULL_HANDLE,
            "m_Instance should have been instanced before this function was called.");

        // Failing to create the debug callback will not throw. If the creation
        // of the debug callback failed, just ignore the call to unregister
        // it.
        if ((m_DebugMessenger != VK_NULL_HANDLE) &&
            (m_CreateDebugMessenger != nullptr) && (m_DestroyDebugMessenger != nullptr))
        {
            m_DestroyDebugMessenger(m_Instance, m_DebugMessenger, nullptr);
        }

        m_CreateDebugMessenger = nullptr;
        m_DestroyDebugMessenger = nullptr;
    }

    bool VulkanGraphicsInstance::InitializeDebugMessengerFunctions()
    {
        KITSUNE_ASSERT(
            m_Instance != VK_NULL_HANDLE,
            "m_Instance should be initialized before calling this function.");

        KITSUNE_ASSERT(
            (m_CreateDebugMessenger == VK_NULL_HANDLE) &&
            (m_DestroyDebugMessenger == VK_NULL_HANDLE),
            "m_CreateDebugMessenger and m_DestroyDebugMessenger have already been "
            "initialized.");

        m_CreateDebugMessenger =
            reinterpret_cast<CreateDebugMessengerFunc>(::vkGetInstanceProcAddr(
                m_Instance, "vkCreateDebugUtilsMessengerEXT"));

        m_DestroyDebugMessenger =
            reinterpret_cast<DestroyDebugMessengerFunc>(::vkGetInstanceProcAddr(
                m_Instance, "vkDestroyDebugUtilsMessengerEXT"));

        return (m_CreateDebugMessenger && m_DestroyDebugMessenger);
    }

    VkBool32 VulkanGraphicsInstance::DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* userData)
    {
        KITSUNE_UNUSED(type);
        KITSUNE_UNUSED(userData);

        KITSUNE_ENGINE_LOG(
            Graphics,
            ToEngineSeverity(severity),
            SourceLocation(),
            data->pMessage);

        return VK_FALSE;
    }

    void VulkanGraphicsInstance::InitializePhysicalDevices()
    {
        Uint32 deviceCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr),
            "Failed to enumerate through all of the physical devices on this device.");

        if (deviceCount == 0)
            throw SystemException("Failed to find devices which support Vulkan.");

        m_Devices.Resize(deviceCount);
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &deviceCount, m_Devices.Data()),
            "Failed to enumerate throught all of the physical devices on this device.");

        for (VkPhysicalDevice device : m_Devices)
            m_DeviceDescriptions.PushBack(GetVulkanDeviceDescription(device));
    }

    GraphicsDeviceDescription VulkanGraphicsInstance::GetVulkanDeviceDescription(
        VkPhysicalDevice device)
    {
        VkPhysicalDeviceIDProperties idProperties;
        idProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;
        idProperties.pNext = nullptr;

        VkPhysicalDeviceProperties2 properties2;
        properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        properties2.pNext = &idProperties;

        const VkPhysicalDeviceProperties& properties = properties2.properties;
        ::vkGetPhysicalDeviceProperties2(device, &properties2);

        return GraphicsDeviceDescription(
            properties.deviceName,
            ToVendorString(properties.vendorID),
            ToGraphicsDeviceType(properties.deviceType),
            GraphicsDeviceUUID(reinterpret_cast<const Byte*>(idProperties.deviceUUID)));
    }
}
