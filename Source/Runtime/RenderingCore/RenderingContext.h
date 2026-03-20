#pragma once

#include "RenderingCore/RenderingDevice.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class RenderingContext : public NonCopyable
    {
    public:
        virtual ~RenderingContext() { /* ... */ }

    public:
        [[nodiscard]]
        virtual Array<RenderingDeviceInformation> GetAvailableDevicesInformation() const = 0;

        [[nodiscard]]
        virtual RenderingDevice* CreateRenderingDevice(Uint32 deviceIndex) = 0;

        virtual void DestroyRenderingDevice(RenderingDevice* device) = 0;
    };
}
