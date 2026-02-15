#pragma once

#include "Application/Screen.h"
#include "Application/Window.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // Used in WindowSpecifications.
    enum class WindowPositionHint
    {
        UsePosition,            //< Use the position argument.
        PrimaryScreenCenter     //< Ignores the position argument and spawns the window
                                //  in the center of the primary screen.
    };

    // Used in WindowSpecifications.
    enum class WindowFlags
    {
        None = 0,               //< None.
        FixedSize = 1 << 0      //< The window cannot be resized.
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowFlags);

    // Defines the window creation settings.
    // - The window position hint controls whether the `Position` member variable will be used.
    //   - WindowPositionHint::UsePosition: Sets the window's top-left position to `Position`.
    //   - WindowPositionHint::PrimaryScreenCenter: Puts the window in the center of the primary screen.
    //     The `Position` member variable is ignored.
    struct WindowSpecifications
    {
        Vector2<Uint32> Size;
        String Title;

        Vector2<Int32> Position;
        WindowPositionHint PositionHint = WindowPositionHint::UsePosition;

        WindowFlags Flags;
    };

    // Encapsulates a platform's implementation of a display server.
    // For example: Desktop Window Manager (DWM) on Windows, and X11 & Wayland on Linux.
    class DisplayManager : public NonCopyable
    {
    public:
        DisplayManager();
        virtual ~DisplayManager();

    public:
        virtual void Update() = 0;

        [[nodiscard]] virtual SharedPtr<Screen> GetPrimaryScreen() const = 0;
        [[nodiscard]] virtual Array<SharedPtr<Screen>> GetScreens() const = 0;

    public:
        // Can't use CreateWindow() here, because.. Windows.
        virtual SharedPtr<Window> MakeWindow(const WindowSpecifications& specs) = 0;

    public:
        // Implemented by the platform-specific implementation.
        // These functions should never be called by engine code.
        static DisplayManager* Create();
        static void Destroy();

    public:
        [[nodiscard]]
        inline static DisplayManager* GetInstance()
        {
            return s_Instance;
        }

    private:
        static DisplayManager* s_Instance;
    };
}
