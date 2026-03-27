#pragma once

#include "Application/Window.h"
#include "Application/Screen.h"

#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    // The API used in the backend for rendering.
    enum class RenderingBackend
    {
        Vulkan
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

    // Specifies configurations that will be used by the display manager.
    struct DisplayManagerSpecifications
    {
        bool Headless = false;

        WindowSpecifications PrimaryWindowSpecs;
        RenderingBackend Backend = RenderingBackend::Vulkan;
    };

    // Manages everything related to input (keyboard, mouse) and windowing.
    class DisplayManager : public NonCopyable
    {
    public:
        virtual ~DisplayManager() = default;

    public:
        virtual void Update() = 0;

    public:
        // These handles are only valid for the frame when the retrieval
        // functions were called.
        // Do not cache these values.
        [[nodiscard]]
        virtual ScreenHandle GetPrimaryScreen() const = 0;

        [[nodiscard]]
        virtual Array<ScreenHandle> GetScreens() const = 0;

    public:
        [[nodiscard]]
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
        static DisplayManager* CreateNullDisplayManager_(
            const DisplayManagerSpecifications& specs);

    private:
        static DisplayManager* s_Instance;
    };
}
