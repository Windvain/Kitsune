#pragma once

#include "GraphicsCore/GpuDevice.h"
#include "GraphicsCore/CommandQueue.h"
#include "GraphicsCore/RenderSurface.h"

#include "Foundation/String/String.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Window;

    // Specifies the graphics API to be used as a backend for interfacing
    // with the GPU.
    enum class GraphicsApi
    {
        Vulkan
    };

    // Specifies the requirements a device has to fulfill to be picked by
    // the GpuInstance::RequestDevice() function.
    struct GpuDeviceRequirements
    {
        SharedPtr<RenderSurface> SupportedSurface;
        GpuDeviceFeature Features = GpuDeviceFeature::None;

        Array<CommandQueueSpecifications> CommandQueues;
        GpuDevicePreference Preference = GpuDevicePreference::Unspecified;
    };

    // Contains information for creating a GPU instance.
    struct GpuInstanceDescription
    {
        bool DebugEnabled = false;
        GraphicsApi Backend;

        String Name;
    };

    // The entry point to the graphics API.
    class GpuInstance : public NonCopyable
    {
    public:
        virtual ~GpuInstance() = default;

    public:
        [[nodiscard]]
        virtual SharedPtr<GpuDevice> RequestDevice(
            const GpuDeviceRequirements& requirements) = 0;

        [[nodiscard]]
        virtual SharedPtr<RenderSurface> RequestSurface(Window* window) = 0;

    public:
        [[nodiscard]]
        virtual GpuInstanceDescription GetDescription() const = 0;

        [[nodiscard]]
        inline bool IsDebugEnabled() const
        {
            return GetDescription().DebugEnabled;
        }

        [[nodiscard]]
        inline GraphicsApi GetBackend() const
        {
            return GetDescription().Backend;
        }

        [[nodiscard]]
        inline String GetName() const
        {
            return GetDescription().Name;
        }

    public:
        [[nodiscard]]
        static SharedPtr<GpuInstance> Create(const GpuInstanceDescription& description);
    };
}
