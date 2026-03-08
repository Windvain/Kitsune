#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class RenderingBackend
    {
        Null,
        Vulkan
    };

    struct GraphicsDeviceSpecifications
    {
        RenderingBackend Backend;
    };

    class GraphicsDevice : public NonCopyable
    {
    public:

    };
}
