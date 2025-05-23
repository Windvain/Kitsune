#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/NonCopyable.h"

#include "RenderingCore/ILogicalDevice.h"
#include "RenderingCore/IPhysicalDevice.h"

#include "RenderingCore/GraphicsBackend.h"

namespace Kitsune
{
    enum class PhysicalDevicePreference
    {
        None,
        HighPerformance,
        PowerSaving,
    };

    class IGraphicsInstance : public NonCopyable
    {
    public:
        virtual ~IGraphicsInstance() { /* ... */ }

    public:
        // Enumerates throughout all physical devices which were found.
        // Return value contains at least one physical device.
        virtual Array<SharedPtr<IPhysicalDevice>> EnumeratePhysicalDevices() = 0;

        // Enumerates through all found physical devices based on the specified preferences.
        // Listed in order, with the most suitable physical device in index 0.
        // Return value contains at least one physical device.
        virtual Array<SharedPtr<IPhysicalDevice>> EnumeratePhysicalDevices(
            PhysicalDevicePreference gpuPref) = 0;

    public:
        virtual SharedPtr<ILogicalDevice> CreateGraphicsDevice(
            const SharedPtr<IPhysicalDevice>& physicalDevice) = 0;

    public:
        virtual GraphicsBackend GetGraphicsBackend() const = 0;
    };

    KITSUNE_API_ SharedPtr<IGraphicsInstance> CreateGraphicsInstance(GraphicsBackend backend);
}
