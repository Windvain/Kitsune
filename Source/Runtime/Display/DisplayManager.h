#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/StringView.h"

#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    // The display's orientation.
    // Note: Setting a display's orientation to 90° twice doesn't make the display
    //       rotate 180°, a.k.a display rotations don't stack.
    enum class DisplayOrientation
    {
        Default,
        Rotated90,
        Rotated180,
        Rotated270
    };

#if defined(KITSUNE_OS_WINDOWS)
    using DisplayId = void*;
#endif

    struct DisplayInformation
    {
        Vector2<Uint32> Size;
        Vector2<Int32> Position;

        Uint32 RefreshRate;
        float Scaling;

        DisplayOrientation Orientation;
        bool MainDisplay;
    };

    struct DisplayManagerConfigurations
    {
        StringView DisplayServer;

        // Ignored when DisplayServer != "Null".
        struct
        {
            Vector2<Uint32> Size;
            Uint32 RefreshRate;
            DisplayOrientation Orientation;
        } NullDisplay;
    };

    class KITSUNE_API DisplayManager
    {
    public:
        virtual ~DisplayManager() = default;

    public:
        static DisplayManager* Initialize(const DisplayManagerConfigurations& configs);
        static void Shutdown();

        [[nodiscard]]
        inline static DisplayManager* GetInstance()
        {
            return s_Instance;
        }

    public:
        virtual void Update(double delta) = 0;

    public:
        [[nodiscard]] virtual Array<DisplayId> GetDisplays() const = 0;
        [[nodiscard]] virtual DisplayId GetMainDisplay() const = 0;

        [[nodiscard]] virtual Usize GetDisplayCount() const = 0;

    public:
        [[nodiscard]]
        virtual DisplayInformation GetDisplayInformation(DisplayId displayID) const = 0;

        [[nodiscard]]
        virtual bool IsDisplayConnected(DisplayId displayID) const = 0;

    public:
        virtual void SetDisplayOrientation(
            DisplayId displayID,
            DisplayOrientation orientation) = 0;

    private:
        static DisplayManager* s_Instance;
    };
}
