#include "GraphicsCore/GpuInstance.h"
#include "Foundation/Logging/Logger.h"

#if defined(KITSUNE_ENABLE_VULKAN_BACKEND)
    #include "GraphicsCore/Vulkan/VulkanGpuInstance.h"
#endif

namespace Kitsune
{
    SharedPtr<GpuInstance> GpuInstance::Create(const GpuInstanceDescription& description)
    {
        switch (description.Backend)
        {
#if defined(KITSUNE_ENABLE_VULKAN_BACKEND)
        case GraphicsApi::Vulkan:
            return MakeShared<VulkanGpuInstance>(description);
#endif
        default:
        {
            KITSUNE_ENGINE_ERROR(
                GraphicsCore,
                "The backend specified during a call to GpuInstance::Create() is "
                "not supported by the operating system or was not compiled "
                "with the engine.");

            // TODO: Add a NULL/no-op implementation of the GpuInstance class.
            // return MakeShared<NullGpuInstance>(description);
            KITSUNE_UNREACHABLE();
        }
        }
    }
}
