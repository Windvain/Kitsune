#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class PhysicalDeviceType
    {
        Integrated,
        Discrete,
        Software
    };

    class IPhysicalDevice : public NonCopyable
    {
    public:
        virtual ~IPhysicalDevice() { /* ... */ }

    public:
        // Gets the name of the physical device.
        virtual String GetName() const = 0;

        // Retrieves the approximated type of the physical device.
        // Can be Integrated, Discrete, or Software.
        virtual PhysicalDeviceType GetType() const = 0;

        // Returns the GPU vendor's PCI ID or ACPI ID.
        virtual Uint32 GetVendorId() const = 0;

        // Gets the name of the physical device's vendor.
        virtual String GetVendorName() const = 0;

    public:
        // Retrieves the amount of video memory dedicated to the video card,
        // not shared with the CPU.
        virtual Usize GetDedicatedVideoMemory() const = 0;

        // Retrieves the amount of system memory that the video card is allowed
        // to allocate by the adapter.
        virtual Usize GetSharedSystemMemory() const = 0;

        // Retrieves the amount of system memory which is not shared with the CPU.
        virtual Usize GetDedicatedSystemMemory() const = 0;

        // Retrieves the total available graphics memory.
        inline Usize GetTotalAvailableGraphicsMemory() const
        {
            return (GetDedicatedSystemMemory() + GetDedicatedVideoMemory() +
                    GetSharedSystemMemory());
        }
    };
}
