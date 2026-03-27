#pragma once

#include "RenderingCore/RenderingDevice.h"

#include "Application/Window.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // An interface for initializing and destroying rendering devices.
    class RenderingContext : public NonCopyable
    {
    public:
        virtual ~RenderingContext() = default;

    public:
        [[nodiscard]]
        virtual RenderingDevice* CreateRenderingDevice(
            Uint32 deviceIndex,
            WindowHandle windowHandle) = 0;

        virtual void DestroyRenderingDevice(RenderingDevice* device) = 0;
    };
}
