#include "GraphicsCore/Vulkan/VulkanGpuInstance.h"

#include "GraphicsCore/Vulkan/VulkanGpuDevice.h"
#include "GraphicsCore/Vulkan/VulkanCommandQueue.h"
#include "GraphicsCore/Vulkan/VulkanRenderSurface.h"

#include "Foundation/Containers/Set.h"
#include "Foundation/Logging/Logger.h"
#include "Foundation/Algorithms/Contains.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    namespace Details
    {
        LogSeverity VulkanToEngine_(VkDebugUtilsMessageSeverityFlagsEXT severity)
        {
            if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                return LogSeverity::Error;
            else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
                return LogSeverity::Warning;
            else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
                return LogSeverity::Info;

            return LogSeverity::Trace;
        }
    }

    VulkanGpuInstance::VulkanGpuInstance(GpuInstanceDescription description)
        : m_Description(Move(description))
    {
        Array<const char*> extensions = GetExtensions_();
        Array<const char*> layers = GetLayers_();

        VerifyExtensionSupport_(extensions);
        VerifyLayerSupport_(layers);

        VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = m_Description.Name.Data(),
            .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
            .pEngineName = "Kitsune Engine",
            .engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1),
            .apiVersion = s_VulkanApiVersion
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

        if (m_Description.DebugEnabled)
            RegisterDebugCallback_();
    }

    VulkanGpuInstance::~VulkanGpuInstance()
    {
        if (m_Description.DebugEnabled)
            UnregisterDebugCallback_();

        m_Devices.Clear();
        m_Surfaces.Clear();

        ::vkDestroyInstance(m_Instance, nullptr);
    }

    SharedPtr<GpuDevice> VulkanGpuInstance::RequestDevice(
        const GpuDeviceRequirements& requirements)
    {
        if (requirements.Preference != GpuDevicePreference::Unspecified)
        {
            KITSUNE_ENGINE_WARN(
                GraphicsCore,
                "The `Preference` member variable of GpuDeviceRequirements was set to "
                "a value other than GpuDevicePreference::Unspecified. The Vulkan "
                "backend does not support using this setting. This warning can be "
                "safely ignored.");
        }

        Uint32 deviceCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr),
            "Failed to enumerate over the available Vulkan physical devices.");

        Array<VkPhysicalDevice> physicalDevices(deviceCount, VkPhysicalDevice());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumeratePhysicalDevices(m_Instance, &deviceCount,
                                         physicalDevices.Data()),
            "Failed to enumerate over the available Vulkan physical devices.");

        for (VkPhysicalDevice physicalDevice : physicalDevices)
        {
            Array<VkDeviceQueueCreateInfo> queueInfos = GetQueueCreateInfos_(
                physicalDevice,
                requirements.CommandQueues,
                requirements.SupportedSurface);

            Array<const char*> extensions = GetDeviceExtensions_();
            if ((queueInfos.Size() != requirements.CommandQueues.Size()) ||
                (!DeviceHasExtensionSupport_(physicalDevice, extensions)) ||
                (!DeviceSupportsFeatures_(physicalDevice)))
            {
                continue;
            }

            return *m_Devices.Insert(
                m_Devices.GetBegin(),
                MakeShared<VulkanGpuDevice>(physicalDevice, queueInfos, extensions));
        }

        throw SystemException(
            "Could not find a suitable Vulkan physical device. "
            "Your GPU might be too old to support the specified requirements!");
    }

    SharedPtr<RenderSurface> VulkanGpuInstance::RequestSurface(Window* window)
    {
        return *m_Surfaces.Insert(
            m_Surfaces.GetBegin(),
            MakeShared<VulkanRenderSurface>(*this, window));
    }

    Array<const char*> VulkanGpuInstance::GetExtensions_()
    {
        Array<const char*> extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME           // For VkSurfaceKHR.
        };

        // For operating system specific implementations of VkSurfaceKHR.
#if defined(KITSUNE_OS_WINDOWS)
        extensions.PushBack(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

        // For VkDebugUtilsMessengerEXT and its creation/deletion functions.
#if !defined(KITSUNE_BUILD_PRODUCTION)
        extensions.PushBack(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        return extensions;
    }

    Array<const char*> VulkanGpuInstance::GetLayers_() const
    {
        Array<const char*> layers;
#if !defined(KITSUNE_BUILD_PRODUCTION)
        // Used in development: Make sure Vulkan usage aligns with the
        // Vulkan standards.
        if (m_Description.DebugEnabled)
            layers.PushBack("VK_LAYER_KHRONOS_validation");
#endif

        return layers;
    }

    void VulkanGpuInstance::VerifyExtensionSupport_(
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

        for (const char* extension : extensions)
        {
            const auto predicate = [&](const VkExtensionProperties& properties) -> bool
            {
                return (StringView(properties.extensionName) == extension);
            };

            if (!Algorithms::ContainsIf(propertiesArray.GetBegin(),
                                        propertiesArray.GetEnd(), predicate))
            {
                throw SystemException(
                    "Extension {0} is not supported by the Vulkan loader or driver.",
                    extension);
            }
        }
    }

    void VulkanGpuInstance::VerifyLayerSupport_(const Array<const char*>& layers)
    {
        Uint32 layerCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr),
            "Failed to enumerate over the instance's layers.");

        Array<VkLayerProperties> propertiesArray(layerCount, VkLayerProperties());
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateInstanceLayerProperties(&layerCount, propertiesArray.Data()),
            "Failed to enumerate over the instance's layers.");

        for (const char* layer : layers)
        {
            const auto predicate = [&](const VkLayerProperties& properties) -> bool
            {
                return (StringView(properties.layerName) == layer);
            };

            if (!Algorithms::ContainsIf(propertiesArray.GetBegin(),
                                        propertiesArray.GetEnd(), predicate))
            {
                throw SystemException(
                    "Layer {0} is not supported by the Vulkan loader or driver.",
                    layer);
            }
        }
    }

    bool VulkanGpuInstance::RetrieveDebugFunctions_()
    {
        KITSUNE_ASSERT(m_Instance != VK_NULL_HANDLE,
                       "m_Instance must exist to call this function.");

        KITSUNE_ASSERT(
            (m_CreateDebugMessenger == VK_NULL_HANDLE) &&
            (m_DestroyDebugMessenger == VK_NULL_HANDLE),
            "Expected m_CreateDebugMessenger and m_DestroyDebugMessenger "
            "to not be set.");

        m_CreateDebugMessenger =
            reinterpret_cast<CreateDebugMessengerFunc>(::vkGetInstanceProcAddr(
                m_Instance, "vkCreateDebugUtilsMessengerEXT"));

        m_DestroyDebugMessenger =
            reinterpret_cast<DestroyDebugMessengerFunc>(::vkGetInstanceProcAddr(
                m_Instance, "vkDestroyDebugUtilsMessengerEXT"));

        if (!m_CreateDebugMessenger || !m_DestroyDebugMessenger)
        {
            KITSUNE_ENGINE_ERROR(
                GraphicsCore,
                "Failed to load one or more of the debug messenger "
                "functions. The engine will not register a debug callback.");

            return false;
        }

        return true;
    }

    void VulkanGpuInstance::RegisterDebugCallback_()
    {
        KITSUNE_ASSERT(m_Instance != VK_NULL_HANDLE,
                       "m_Instance must exist to call this function.");

        if (!RetrieveDebugFunctions_())
            return;

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
            .pfnUserCallback = &VulkanGpuInstance::DebugCallback_,
            .pUserData = nullptr
        };

        VkResult result = m_CreateDebugMessenger(m_Instance, &createInfo, nullptr,
                                                 &m_DebugMessenger);
        if (result != VK_SUCCESS)
        {
            KITSUNE_ENGINE_ERROR(
                GraphicsCore,
                "Failed to create the Vulkan debug callback. This could be "
                "due to the system being low in memory.");
        }
    }

    void VulkanGpuInstance::UnregisterDebugCallback_()
    {
        KITSUNE_ASSERT(m_Instance != VK_NULL_HANDLE,
                       "m_Instance must exist to call this function.");

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

    VkBool32 VulkanGpuInstance::DebugCallback_(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* userData)
    {
        KITSUNE_UNUSED(type);
        KITSUNE_UNUSED(userData);

        KITSUNE_ENGINE_LOG(
            GraphicsCore,
            Details::VulkanToEngine_(severity), SourceLocation(),
            data->pMessage);

        return VK_FALSE;
    }

    Array<VkDeviceQueueCreateInfo> VulkanGpuInstance::GetQueueCreateInfos_(
        VkPhysicalDevice physicalDevice,
        const Array<CommandQueueSpecifications>& commandQueues,
        const SharedPtr<RenderSurface>& supportedSurface)
    {
        Uint32 queueFamilyCount = 0;
        ::vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount,
                                                    nullptr);
        if (queueFamilyCount == 0)
            return { /* ... */ };

        Array<VkQueueFamilyProperties2> familyProperties(queueFamilyCount, {
                .sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2,
                .pNext = nullptr,
                .queueFamilyProperties = VkQueueFamilyProperties()
            });

        ::vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyCount,
                                                    familyProperties.Data());

        Array<VkDeviceQueueCreateInfo> queueCreateInfos;
        Set<Uint32> uniqueFamilies;

        for (auto& queueSpecifications : commandQueues)
        {
            if (queueSpecifications.Count == 0)
            {
                KITSUNE_ENGINE_WARN(
                    GraphicsCore,
                    "Tried to create a device with CommandQueueSpecifications::Count "
                    "set to zero. This queue family will not be created.");

                continue;
            }

            VkQueueFlags flags = Details::EngineToVulkan_(queueSpecifications.Type);
            for (Uint32 index = 0; index < familyProperties.Size(); ++index)
            {
                auto properties = familyProperties[index].queueFamilyProperties;
                if ((properties.queueFlags & flags) != flags)
                    continue;

                if (queueSpecifications.Count > properties.queueCount)
                    continue;

                // If CommandQueueFlag::Presentable was specified, make sure the surface
                // passed in is supported.
                if ((queueSpecifications.Flags & CommandQueueFlag::Presentable) ==
                    CommandQueueFlag::Presentable)
                {
                    if (!QuerySurfaceSupport_(physicalDevice, index, supportedSurface))
                        continue;
                }

                auto [iterator_, result] = uniqueFamilies.Insert(index);
                if (result)
                {
                    queueCreateInfos.PushBack({
                        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,
                        .queueFamilyIndex = index,
                        .queueCount = queueSpecifications.Count,
                        .pQueuePriorities = queueSpecifications.Priorities.Data()
                    });

                    break;
                }
            }
        }

        return queueCreateInfos;
    }

    bool VulkanGpuInstance::QuerySurfaceSupport_(
        VkPhysicalDevice physicalDevice,
        Uint32 familyIndex,
        const SharedPtr<RenderSurface>& surface)
    {
        // The surface to check was not specified, return true.
        if (surface == nullptr)
            return true;

        VkBool32 supportsPresentation;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkGetPhysicalDeviceSurfaceSupportKHR(
                physicalDevice,
                familyIndex,
                Details::GetVulkanHandle_(surface),
                &supportsPresentation),
            "Failed to retrieve the presentation state of a Vulkan "
            "physical device.");

        return supportsPresentation;
    }

    Array<const char*> VulkanGpuInstance::GetDeviceExtensions_()
    {
        return {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME     // For VkSwapchainKHR.
        };
    }

    bool VulkanGpuInstance::DeviceHasExtensionSupport_(
        VkPhysicalDevice device,
        const Array<const char*>& deviceExtensions)
    {
        Uint32 extensionCount;
        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                                   nullptr),
            "Failed to enumerate through the Vulkan physical device's "
            "supported extensions.");

        Array<VkExtensionProperties> propertiesArray(
            extensionCount, VkExtensionProperties());

        KITSUNE_VK_THROW_IF_FAIL(
            ::vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                                   propertiesArray.Data()),
            "Failed to enumerate through the Vulkan physical device's "
            "supported extensions.");

        for (const char* extension : deviceExtensions)
        {
            const auto predicate = [&](const VkExtensionProperties& properties) -> bool
            {
                return (StringView(properties.extensionName) == extension);
            };

            if (!Algorithms::ContainsIf(
                    propertiesArray.GetBegin(), propertiesArray.GetEnd(),
                    predicate))
            {
                return false;
            }
        }

        return true;
    }

    bool VulkanGpuInstance::DeviceSupportsFeatures_(VkPhysicalDevice device)
    {
        // These names are even longer than Java class names...
        using Vulkan13Features = VkPhysicalDeviceVulkan13Features;

        Vulkan13Features vulkan13Features = { /* ... */ };
        vulkan13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vulkan13Features.pNext = nullptr;
        vulkan13Features.synchronization2 = VK_TRUE;

        VkPhysicalDeviceFeatures deviceFeatures = { /* ... */ };

#if !defined(KITSUNE_BUILD_PRODUCTION)
        deviceFeatures.fillModeNonSolid = VK_TRUE;
#endif

        VkPhysicalDeviceFeatures2 features2 = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = &vulkan13Features,
            .features = deviceFeatures
        };

        ::vkGetPhysicalDeviceFeatures2(device, &features2);

        const auto* current = reinterpret_cast<const VkBaseInStructure*>(&features2);
        bool supported = true;

        while (current != nullptr)
        {
            switch (current->sType)
            {
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES:
            {
                auto* features = reinterpret_cast<const Vulkan13Features*>(current);
                if (!features->synchronization2 || !features->dynamicRendering)
                    supported = false;

                break;
            }
            case VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2:
            {
                auto* features = reinterpret_cast<const VkPhysicalDeviceFeatures2*>(
                    current);

                if (features->features.fillModeNonSolid == VK_FALSE)
                    supported = false;

                break;
            }
            default:
                break;
            }

            if (!supported)
                return false;

            current = current->pNext;
        }

        return true;
    }
}
