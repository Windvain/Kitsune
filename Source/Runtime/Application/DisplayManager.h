#pragma once

#include "Application/Window.h"
#include "Application/Screen.h"

#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    // Specifies configurations that will be used by the display manager.
    struct DisplayManagerSpecifications
    {
        bool Headless = false;
    };

    // Contains settings used to create a window.
    struct WindowSpecifications
    {
        Vector2<Uint32> Size;
        Vector2<Int32> Position;

        String Title;

        WindowMode Mode = WindowMode::Windowed;
        WindowFlags Flags = WindowFlags::None;
    };

    // Manages everything related to input (keyboard, mouse) and windowing.
    class DisplayManager : public NonCopyable
    {
    public:
        virtual ~DisplayManager()
        {
        }

    public:
        virtual void Update() = 0;

        // These handles are only valid for the frame when the retrieval functions were called.
        // Do not cache these values.
        virtual ScreenHandle GetPrimaryScreen() const = 0;
        virtual Array<ScreenHandle> GetScreens() const = 0;

    public:
        virtual WindowHandle MakeWindow(const WindowSpecifications& specs) = 0;
        virtual void DestroyWindow(WindowHandle window) = 0;

        virtual WindowHandle GetPrimaryWindow() const = 0;

    public:
        // These functions (i.e. Initialize() and Shutdown()) should not be called by
        // client code. They are only meant for usage in the engine initialization code.
        static DisplayManager* Initialize(const DisplayManagerSpecifications& specs);
        static void Shutdown();

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
