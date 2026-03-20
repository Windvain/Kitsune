#include "RenderingCore/Vulkan/VulkanRenderingContext.h"
#include "RenderingCore/RenderingDevice.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Algorithms/Contains.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
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

    VulkanRenderingContext::VulkanRenderingContext(const StringView applicationName)
        : m_ApplicationName(applicationName)
    {
        KITSUNE_ENGINE_INFO_FORMAT_(
            "Creating a Vulkan rendering context with the application name \"{0}\".",
            m_ApplicationName);

#pragma region Instance Creation
        Array<const char*> extensions = GetRequestedExtensions_();
        Array<const char*> layers = GetRequestedLayers_();

        VerifyExtensions_(extensions);
        VerifyLayers_(layers);

        m_Extensions = extensions;
        m_Layers = layers;

        KITSUNE_ENGINE_INFO_FORMAT_(
            "Instance-level extensions requested: {0}", extensions.Size());

        for (const char* extension : extensions)
            KITSUNE_ENGINE_INFO_FORMAT_("\t-> {0}", extension);

        KITSUNE_ENGINE_INFO_FORMAT_("Layers requested: {0}", layers.Size());
        for (const char* layer : layers)
            KITSUNE_ENGINE_INFO_FORMAT_("\t-> {0}", layer);

        VkApplicationInfo applicationInfo = { /* ... */ };
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = m_ApplicationName.Raw();
        applicationInfo.pEngineName = "Kitsune Engine";
        applicationInfo.apiVersion = VK_API_VERSION_1_4;

        VkInstanceCreateInfo instanceCreateInfo = { /* ... */ };
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        instanceCreateInfo.enabledExtensionCount = static_cast<std::uint32_t>(m_Extensions.Size());
        instanceCreateInfo.ppEnabledExtensionNames = m_Extensions.Data();

        instanceCreateInfo.enabledLayerCount = static_cast<std::uint32_t>(m_Layers.Size());
        instanceCreateInfo.ppEnabledLayerNames = m_Layers.Data();

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance),
            "Failed to create a Vulkan instance.");

    #if !defined(KITSUNE_BUILD_PRODUCTION)
        RegisterDebugCallback_();
    #endif
#pragma endregion

#pragma region Physical Device Enumeration
        std::uint32_t physicalDeviceCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr),
            "Failed to enumerate over the physical devices on the system.");

        Array<VkPhysicalDevice> physicalDevices(physicalDeviceCount, VkPhysicalDevice());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.Data()),
            "Failed to enumerate over the physical devices on the system.");

        m_PhysicalDevices = physicalDevices;

        for (Uint32 i = 0; i < m_PhysicalDevices.Size(); ++i)
        {
            VkPhysicalDevice physicalDevice = m_PhysicalDevices[i];
            VkPhysicalDeviceProperties deviceProperties;

            ::vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

            RenderingDeviceInformation deviceInfo;
            deviceInfo.Index = i;
            deviceInfo.Name = deviceProperties.deviceName;

            switch (deviceProperties.deviceType)
            {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                deviceInfo.Type = RenderingDeviceType::Integrated;
                break;

            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                deviceInfo.Type = RenderingDeviceType::Discrete;
                break;

            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                deviceInfo.Type = RenderingDeviceType::Virtual;
                break;

            case VK_PHYSICAL_DEVICE_TYPE_CPU:
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            default:        // VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM
                deviceInfo.Type = RenderingDeviceType::Other;
                break;
            }

            m_PhysicalDevicesInfo.PushBack(deviceInfo);
        }

#pragma endregion

        KITSUNE_ENGINE_INFO_("Successfully created the Vulkan rendering context.");
    }

    VulkanRenderingContext::~VulkanRenderingContext()
    {
        KITSUNE_ASSERT(
            m_Instance != VK_NULL_HANDLE,
            "Attempted to destroy a VkInstance which had not been initialized or has already "
            "been destroyed.");

#if !defined(KITSUNE_BUILD_PRODUCTION)
        if (m_DebugMessenger != VK_NULL_HANDLE)
            UnregisterDebugCallback_();
#endif

        ::vkDestroyInstance(m_Instance, nullptr);

        KITSUNE_ENGINE_INFO_FORMAT_(
            "Destroyed the Vulkan rendering context with application name \"{0}\".",
            m_ApplicationName);
    }

    Array<RenderingDeviceInformation> VulkanRenderingContext::GetAvailableDevicesInformation() const
    {
        return m_PhysicalDevicesInfo;
    }

    RenderingDevice* VulkanRenderingContext::CreateRenderingDevice(Uint32 deviceIndex)
    {
        KITSUNE_ENGINE_INFO_FORMAT_(
            "Creating a RenderingDevice with physical device index #{0}.",
            deviceIndex);

        VulkanRenderingDevice* renderingDevice = Memory::New<VulkanRenderingDevice>(
            m_PhysicalDevices[deviceIndex],
            m_PhysicalDevicesInfo[deviceIndex]);

        m_RenderingDevices.PushBack(renderingDevice);

        KITSUNE_ENGINE_INFO_("The rendering device has been created.");
        return renderingDevice;
    }

    void VulkanRenderingContext::DestroyRenderingDevice(RenderingDevice* device)
    {
        auto iter = Algorithms::Find(m_RenderingDevices.GetBegin(), m_RenderingDevices.GetEnd(),
                                     device);

        if (iter == m_RenderingDevices.GetEnd())
        {
            throw InvalidArgumentException("Failed to destroy the rendering device. The "
                                           "device specified was not created by "
                                           "this RenderingContext.");
        }

        Memory::Delete(device);
        m_RenderingDevices.Remove(iter);

        KITSUNE_ENGINE_INFO_FORMAT_("Destroyed the RenderingDevice at {0}.", device);
    }

    Array<const char*> VulkanRenderingContext::GetRequestedExtensions_()
    {
        Array<const char*> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME           // For VkSurfaceKHR.
        };

#if defined(KITSUNE_OS_WINDOWS)
        // For Win32 implementation of VkSurfaceKHR.
        extensions.PushBack(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#if !defined(KITSUNE_BUILD_PRODUCTION)
        // For VkDebugUtilsMessengerEXT and its creation/deletion functions.
        extensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }

    Array<const char*> VulkanRenderingContext::GetRequestedLayers_()
    {
        Array<const char*> layers;

#if !defined(KITSUNE_BUILD_PRODUCTION)
        layers.PushBack("VK_LAYER_KHRONOS_validation");
#endif

        return layers;
    }

    void VulkanRenderingContext::VerifyExtensions_(const Array<const char*>& extensions)
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

        for (const char* extensionName : extensions)
        {
            const auto predicate = [extensionName](const VkExtensionProperties& properties) -> bool
            {
                return (StringView(properties.extensionName) == extensionName);
            };

            if (!Algorithms::ContainsIf(extensionProperties.GetBegin(), extensionProperties.GetEnd(),
                                        predicate))
            {
                throw SystemException("One or more of the requested extensions are not supported.");
            }
        }
    }

    void VulkanRenderingContext::VerifyLayers_(const Array<const char*>& layers)
    {
        std::uint32_t layerCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr),
            "Failed to enumerate over the instance's layers.");

        Array<VkLayerProperties> layerProperties(layerCount, VkLayerProperties());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.Data()),
            "Failed to enumerate over the instance's layers.");;

        for (const char* layerName : layers)
        {
            const auto predicate = [&](const VkLayerProperties& properties) -> bool
            {
                return (StringView(properties.layerName) == layerName);
            };

            if (!Algorithms::ContainsIf(layerProperties.GetBegin(), layerProperties.GetEnd(),
                                        predicate))
            {
                throw SystemException(
                    "Could not find one or more of the engine's "
                    "requested layers.");
            }
        }
    }

    void VulkanRenderingContext::RetrieveDebugFunctions_()
    {
        KITSUNE_ASSERT(
            m_Instance != VK_NULL_HANDLE,
            "The VkInstance has not yet been instanced.");

        m_CreateDebugMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            ::vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));

        m_DestroyDebugMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            ::vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));

        if ((m_CreateDebugMessenger == nullptr) || (m_DestroyDebugMessenger == nullptr))
        {
            KITSUNE_ENGINE_ERROR_("Failed to load one or more of the debug messenger functions. "
                                  "The engine will not register a debug callback.");

            return;
        }
    }

    void VulkanRenderingContext::RegisterDebugCallback_()
    {
        RetrieveDebugFunctions_();
        if (m_CreateDebugMessenger == nullptr)
            return;

        VkDebugUtilsMessengerCreateInfoEXT dbgCallbackCreateInfo = { /* ... */ };
        dbgCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        dbgCallbackCreateInfo.pfnUserCallback = &VulkanRenderingContext::DebugCallback_;
        dbgCallbackCreateInfo.pUserData = nullptr;

        dbgCallbackCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        dbgCallbackCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        KITSUNE_VK_THROW_IF_FAIL(
            m_CreateDebugMessenger(m_Instance, &dbgCallbackCreateInfo, nullptr, &m_DebugMessenger),
            "Failed to create the Vulkan debug callback. This is probably "
            "due to the system being out of memory.");

        KITSUNE_ENGINE_INFO_("Registered the Vulkan debug callback.");
    }

    void VulkanRenderingContext::UnregisterDebugCallback_()
    {
        if (m_DestroyDebugMessenger == nullptr)
            return;

        m_DestroyDebugMessenger(m_Instance, m_DebugMessenger, nullptr);
        KITSUNE_ENGINE_INFO_("Destroyed the Vulkan debug messenger.");
    }

    VkBool32 VulkanRenderingContext::DebugCallback_(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* data,
                                                    void* userData)
    {
        KITSUNE_UNUSED(type);
        KITSUNE_UNUSED(userData);

        Log("Kitsune", ToLoggingSeverity(severity), SourceLocation(), data->pMessage);
        return VK_FALSE;
    }
}
