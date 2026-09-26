#include "Graphics/GraphicsInstance.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

#if defined(KITSUNE_ENABLE_VULKAN_BACKEND)
    #include "Graphics/Vulkan/VulkanGraphicsInstance.h"
#endif

namespace Kitsune
{
    GraphicsInstance* GraphicsInstance::s_Instance = nullptr;

    GraphicsInstance* GraphicsInstance::Initialize(
        const GraphicsInstanceConfigurations& configs)
    {
        KITSUNE_ENGINE_INFO_FORMAT(
            Graphics,
            "Initializing \"{0}\" graphics instance as the rendering backend.",
            configs.Backend);

        if (configs.Backend == "Null")
        {
            // TODO: Add a null backend;
            return nullptr;
        }
#if defined(KITSUNE_ENABLE_VULKAN_BACKEND)
        else if (configs.Backend == "Vulkan")
        {
            s_Instance = Memory::New<VulkanGraphicsInstance>(
                configs.DebugEnabled,
                configs.DebugName);
        }
#endif
        else
        {
            throw InvalidArgumentException(
                "The specified graphics backend \"{0}\" was not compiled with the "
                "engine.",
                configs.Backend);
        }

        return s_Instance;
    }

    void GraphicsInstance::Shutdown()
    {
        KITSUNE_ENGINE_INFO(Display, "Destroying the graphics instance.");
        Memory::Delete(s_Instance);
    }
}
