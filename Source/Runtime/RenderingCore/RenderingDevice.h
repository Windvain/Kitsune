#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class RenderingDeviceFeatures
    {
        /* ... */
    };

    enum class RenderingDeviceType
    {
        Other,
        Integrated,
        Discrete,
        Virtual
    };

    struct RenderingDeviceInformation
    {
        String Name;

        RenderingDeviceType Type;
        RenderingDeviceFeatures Features;
    };

    class RenderingDevice : public NonCopyable
    {
    public:
        virtual ~RenderingDevice() { /* ... */ }

    public:
        virtual RenderingDeviceInformation GetInformation() const = 0;
    };
}
