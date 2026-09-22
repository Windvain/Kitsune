#pragma once

#include "Display/Window.h"
#include "Display/Display.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    struct DisplayManagerConfigurations
    {
        String DisplayServer;
        String WindowClassName;     // NOTE: Only applies on Windows, will be ignored
                                    // on other platforms.

        // Only applies when DisplayServer == "Null".
        struct
        {
            Vector2<Uint32> Size = { 1920, 1080 };
            Uint32 RefreshRate = 60;
            float Scaling = 1.0f;
        } VirtualDisplay;
    };

    class DisplayManager
    {
    public:
        virtual ~DisplayManager() = default;

    public:
        KITSUNE_API static DisplayManager* Initialize(
            const DisplayManagerConfigurations& configs);

        KITSUNE_API static void Shutdown();

        [[nodiscard]]
        inline static DisplayManager* GetInstance()
        {
            return s_Instance;
        }

    public:
        virtual void Update() = 0;

    public:
        [[nodiscard]]
        virtual ScopedPtr<Window> CreateWindow(
            const WindowConfigurations& configurations) = 0;

    public:
        // The returned array from this function will never be empty.
        // The first member of the array (i.e. array[0]) is the main display.
        [[nodiscard]]
        virtual Array<SharedPtr<Display>> GetDisplays() const = 0;

        [[nodiscard]]
        virtual SharedPtr<Display> GetMainDisplay() const = 0;

    private:
        static DisplayManager* s_Instance;
    };
}
