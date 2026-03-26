#include "RenderingCore/Vulkan/VulkanRenderingContext.h"
#include "RenderingCore/Vulkan/VulkanRenderingDevice.h"

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

    VulkanRenderingContext::VulkanRenderingContext()
    {
        KITSUNE_ENGINE_INFO(VulkanRendering, "Creating a Vulkan rendering context...");

#pragma region Instance Creation
        Array<const char*> extensions = GetRequestedExtensions_();
        Array<const char*> layers = GetRequestedLayers_();

        VerifyExtensionsSupport_(extensions);
        VerifyLayersSupport_(layers);

        KITSUNE_ENGINE_INFO_FORMAT(
            VulkanRendering,
            "Instance-level extensions requested: {0}", extensions.Size());

        for (const char* extension : extensions)
        {
            KITSUNE_ENGINE_INFO_FORMAT(
                VulkanRendering,
                "\t-> {0}", extension);
        }

        KITSUNE_ENGINE_INFO_FORMAT(
            VulkanRendering,
            "Layers requested: {0}", layers.Size());

        for (const char* layer : layers)
            KITSUNE_ENGINE_INFO_FORMAT(VulkanRendering, "\t-> {0}", layer);

        VkApplicationInfo applicationInfo = { /* ... */ };
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Kitsune Engine Application";
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
            ::vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance),
            "Failed to create a Vulkan instance.");

    #if !defined(KITSUNE_BUILD_PRODUCTION)
        RegisterDebugCallback_();
    #endif
#pragma endregion

#pragma region Physical Device Enumeration
        std::uint32_t physicalDeviceCount;
        Array<VkPhysicalDevice> physicalDevices;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr),
            "Failed to enumerate over the physical devices on the system.");

        if (physicalDeviceCount == 0)
            throw SystemException("Could not find a physical device which supports Vulkan.");

        physicalDevices.Resize(physicalDeviceCount);
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.Data()),
            "Failed to enumerate over the physical devices on the system.");

        m_PhysicalDevices = physicalDevices;
#pragma endregion

        KITSUNE_ENGINE_INFO(
            VulkanRendering,
            "Successfully created the Vulkan rendering context.");
    }

    VulkanRenderingContext::~VulkanRenderingContext()
    {
        KITSUNE_ASSERT(
            m_Instance != VK_NULL_HANDLE,
            "Attempted to destroy a VkInstance which had not been initialized or has already "
            "been destroyed.");

        if (!m_RenderingDevices.IsEmpty())
        {
            KITSUNE_ENGINE_ERROR(
                VulkanRendering,
                "One or more rendering devices have not been deleted, a memory leak or "
                "resource leak might occur.");
        }

#if !defined(KITSUNE_BUILD_PRODUCTION)
        if (m_DebugMessenger != VK_NULL_HANDLE)
            UnregisterDebugCallback_();
#endif

        ::vkDestroyInstance(m_Instance, nullptr);

        KITSUNE_ENGINE_INFO(
            VulkanRendering,
            "Destroyed the Vulkan rendering context.");
    }

    RenderingDevice* VulkanRenderingContext::CreateRenderingDevice(
        Uint32 deviceIndex,
        WindowHandle windowHandle)
    {
        VkPhysicalDevice physicalDevice = m_PhysicalDevices[deviceIndex];
        VkPhysicalDeviceProperties properties;

        ::vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        KITSUNE_ENGINE_INFO_FORMAT(
            VulkanRendering,
            "Creating a RenderingDevice with physical device index #{0}. ({1})",
            deviceIndex, properties.deviceName);

        m_RenderingDevices.PushBack(
            MakeScoped<VulkanRenderingDevice>(
                m_Instance,
                m_PhysicalDevices[deviceIndex],
                windowHandle));

        KITSUNE_ENGINE_INFO(
            VulkanRendering,
            "The rendering device has been created.");

        return m_RenderingDevices.Back().Get();
    }

    void VulkanRenderingContext::DestroyRenderingDevice(RenderingDevice* device)
    {
        auto iter = Algorithms::Find(
            m_RenderingDevices.GetBegin(), m_RenderingDevices.GetEnd(),
            device);

        if (iter == m_RenderingDevices.GetEnd())
        {
            throw InvalidArgumentException(
                "Failed to destroy the rendering device. The device specified was not "
                "created by this RenderingContext.");
        }

        m_RenderingDevices.Remove(iter);
        KITSUNE_ENGINE_INFO_FORMAT(
            VulkanRendering,
            "Destroyed the RenderingDevice at {0}.", device);
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

    void VulkanRenderingContext::VerifyExtensionsSupport_(const Array<const char*>& extensions)
    {
        std::uint32_t extensionCount;
        Array<VkExtensionProperties> extensionProperties;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr),
            "Failed to enumerate over the list of supported extensions.");

        extensionProperties.Resize(extensionCount);
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

    void VulkanRenderingContext::VerifyLayersSupport_(const Array<const char*>& layers)
    {
        std::uint32_t layerCount;
        Array<VkLayerProperties> layerProperties;

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr),
            "Failed to enumerate over the instance's layers.");

        layerProperties.Resize(layerCount);

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
            KITSUNE_ENGINE_ERROR(
                VulkanRendering,
                "Failed to load one or more of the debug messenger functions. "
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

        KITSUNE_ENGINE_INFO(
            VulkanRendering,
            "Registered the Vulkan debug callback.");
    }

    void VulkanRenderingContext::UnregisterDebugCallback_()
    {
        if (m_DestroyDebugMessenger == nullptr)
            return;

        m_DestroyDebugMessenger(m_Instance, m_DebugMessenger, nullptr);
        KITSUNE_ENGINE_INFO(
            VulkanRendering,
            "Destroyed the Vulkan debug messenger.");
    }

    VkBool32 VulkanRenderingContext::DebugCallback_(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* data,
                                                    void* userData)
    {
        KITSUNE_UNUSED(type);
        KITSUNE_UNUSED(userData);

        KITSUNE_ENGINE_LOG(
            VulkanRendering,
            ToLoggingSeverity(severity), SourceLocation(), data->pMessage);

        return VK_FALSE;
    }
}
