#pragma once

#include "GraphicsCore/GpuDevice.h"
#include "GraphicsCore/CommandQueue.h"
#include "GraphicsCore/RenderSurface.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Window;

    // Specifies the preferences for picking a GPU. This enum is intended
    // to be used with the DX12 backend, but other rendering backends can
    // choose to honor these preferences.
    enum class GpuDevicePreference
    {
        Unspecified,
        BatterySaver,
        HighPerformance
    };

    // Specifies the graphics API to be used as a backend for interfacing
    // with the GPU.
    enum class GraphicsApi
    {
        Vulkan
    };

    // Specifies the extensions which a GPU can support.
    enum class GpuDeviceExtension
    {
        None = 0
    };

    // Specifies flags for the command queue.
    enum class CommandQueueFlag
    {
        None = 0,
        Presentable = 1 << 0    //< The command queue can be used for presentation.
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(GpuDeviceExtension);
    KITSUNE_OVERLOAD_FLAGS_OPERATORS(CommandQueueFlag);

    // Specifies the command queue(s) to be requested from the device.
    struct CommandQueueSpecifications
    {
        CommandQueueType Type;
        Uint32 Count;

        CommandQueueFlag Flags;
        Array<float> Priorities = { 1.0f };
    };

    // Specifies the requirements a device has to fulfill to be picked by
    // the GpuInstance::RequestDevice() function.
    struct GpuDeviceRequirements
    {
        SharedPtr<RenderSurface> SupportedSurface;
        GpuDeviceExtension Extensions = GpuDeviceExtension::None;

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
