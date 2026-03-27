#pragma once

#include "Foundation/String/String.h"
#include "Foundation/String/Format.h"

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // Features supported by the rendering device.
    enum class RenderingDeviceFeatures
    {
    };

    // The type of the physical device (GPU) used for rendering.
    enum class RenderingDeviceType
    {
        Other,          //< The rendering backend doesn't support retrieving
                        //  the physical device type, or isn't able to ascertain the
                        //  type of the physical device.
        Integrated,     //< The physical device is an integrated GPU.
        Discrete,       //< The physical device is a discrete GPU.
        Virtual         //< The physical device is virtualized.
    };

    // Contains information about the device used for rendering.
    struct RenderingDeviceInformation
    {
        String Name;

        RenderingDeviceType Type;
        RenderingDeviceFeatures Features;
    };

    // A class which provides an interface to low-level rendering functions.
    class RenderingDevice : public NonCopyable
    {
    public:
        virtual ~RenderingDevice() = default;

    public:
        [[nodiscard]]
        virtual RenderingDeviceInformation GetInformation() const = 0;
    };

    template<>
    class Formatter<RenderingDeviceType, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            RenderingDeviceType renderingDeviceType,
            const FormatContext<Iter>& context)
        {
            String typeString = ToString_(renderingDeviceType);
            return Algorithms::Copy(typeString.GetBegin(), typeString.GetEnd(),
                                    context.GetOutput());
        }

    private:
        inline static String ToString_(RenderingDeviceType type)
        {
            switch (type)
            {
            case RenderingDeviceType::Other:      return "Other";
            case RenderingDeviceType::Integrated: return "Integrated";
            case RenderingDeviceType::Discrete:   return "Discrete";
            case RenderingDeviceType::Virtual:    return "Virtual";
            }

            KITSUNE_UNREACHABLE();
        }
    };

    template<>
    class Formatter<RenderingDevice, char>
    {
    public:
        template<OutputIterator<const char&> Iter>
        inline static Iter Format(
            const RenderingDevice& renderingDevice,
            const FormatContext<Iter>& context)
        {
            RenderingDeviceInformation deviceInfo =
                renderingDevice.GetInformation();

            auto output = FormatTo(
                context.GetOutput(),
                "[ Name: {0}, Type: {1} ]",
                deviceInfo.Name, deviceInfo.Type);

            return output;
        }
    };
}
