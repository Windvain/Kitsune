#pragma once

#include "Application/Window.h"
#include "Foundation/String/String.h"

#include "Application/DisplayManager.h"
#include "Application/CommandLineArguments.h"

KITSUNE_PUSH_COMPILER_WARNINGS()

KITSUNE_IGNORE_MSVC_WARNING(4100)       // warning 4100: 'meow' unreferenced parameter.
KITSUNE_IGNORE_CLANG_WARNING(-Wunused-parameter)

namespace Kitsune
{
    // Specifies settings for application creation.
    // For details on the window member variables, look at `WindowSpecifications`.
    struct ApplicationSpecifications
    {
        String Name;
        String Description;

        String Version;
        bool Headless = false;

        Vector2<Uint32> ViewportSize = { 1280, 720 };
        Vector2<Int32> WindowPosition = { 0, 0 };

        WindowMode WindowMode = WindowMode::Windowed;
        WindowFlags WindowFlags = WindowFlags::None;
    };

    class Application : public NonCopyable
    {
    public:
        Application(const ApplicationSpecifications& specs);
        virtual ~Application();

    public:
        virtual void OnUpdate() { /* ... */ }

    public:
        void Update();

    public:
        [[nodiscard]] inline String GetName()        const { return m_Name; }
        [[nodiscard]] inline String GetDescription() const { return m_Description; }
        [[nodiscard]] inline String GetVersion()     const { return m_Version; }

    public:
        inline static Application* GetInstance()
        {
            return s_Instance;
        }

    private:
        static Application* s_Instance;

    private:
        String m_Name;
        String m_Description;
        String m_Version;

        WindowHandle m_PrimaryWindow;
        DisplayManager* m_DisplayManager;
    };

    // Should be defined in client code.
    extern Application* CreateApplication(const CommandLineArguments& cmdLineArgs);
}

KITSUNE_POP_COMPILER_WARNINGS()
