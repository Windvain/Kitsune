#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class Window;

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

    private:
        static GraphicsInstance* s_Instance;
    };
}
