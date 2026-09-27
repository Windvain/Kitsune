#pragma once

#include "Graphics/GraphicsDevice.h"

#include "Foundation/String/String.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum GraphicsDevicePreference
    {
        PowerSaving,
        HighPerformance
    };

    // Contains information for creating a graphics instance.
    struct GraphicsInstanceConfigurations
    {
        String Backend;

        String DebugName;
        bool DebugEnabled = false;
    };

    // The entry point to the graphics API.
    class GraphicsInstance : public NonCopyable
    {
    public:
        virtual ~GraphicsInstance() = default;

    public:
        KITSUNE_API static GraphicsInstance* Initialize(
            const GraphicsInstanceConfigurations& configs);

        KITSUNE_API static void Shutdown();

    public:
        [[nodiscard]]
        inline static GraphicsInstance* GetInstance()
        {
            return s_Instance;
        }

    public:
        [[nodiscard]]
        virtual Array<GraphicsDeviceDescription> GetDeviceDescriptions() const = 0;

    public:
        [[nodiscard]]
        virtual SharedPtr<GraphicsDevice> RequestDevice() = 0;

        [[nodiscard]]
        virtual SharedPtr<GraphicsDevice> RequestDevice(
            const GraphicsDeviceUUID& uuid) = 0;

        [[nodiscard]]
        virtual SharedPtr<GraphicsDevice> RequestDevice(
            GraphicsDevicePreference preference) = 0;

    private:
        KITSUNE_API static GraphicsInstance* s_Instance;
    };
}
