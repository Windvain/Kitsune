#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Memory/SharedPtr.h"

#include "Foundation/Utilities/Process.h"
#include "Foundation/Diagnostics/Assert.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/IMonitor.h"

#include "ApplicationCore/CommandLineArguments.h"

KITSUNE_PUSH_COMPILER_WARNINGS()

KITSUNE_IGNORE_MSVC_WARNING(4100)       // 'XXX': Unreferenced format parameter (in virtual functions)
KITSUNE_IGNORE_CLANG_WARNING(-Wunused-parameter)

namespace Kitsune
{
    enum class WindowPositionHint
    {
        UsePosition,
        PrimaryScreenCenter,
        ScreenCenter
    };

    struct ApplicationSpecs
    {
        Vector2<Uint32> ViewportSize = { 640, 480 };
        Vector2<Int32> WindowPosition;

        WindowState WindowState = WindowState::Windowed;
        WindowPositionHint WindowPositionHint = WindowPositionHint::ScreenCenter;

        Index WindowMonitorIndex = 0;

        bool WindowResizable = true;
        bool Headless = false;

        String Name;
        String VersionString;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecs& specs);
        virtual ~Application();

    public:
        void Update();

    public:
        virtual void OnUpdate() { /* ... */ }

        virtual void OnWindowResize(const Vector2<Uint32>& size) { /* ... */ }
        virtual void OnWindowMove(const Vector2<Int32>& pos) { /* ... */ }

        virtual void OnWindowMaximize() { /* ... */ }
        virtual void OnWindowMinimize() { /* ... */ }

    public:
        [[nodiscard]]
        inline String GetName() const
        {
            return m_ApplicationSpecs.Name;
        }

        [[nodiscard]]
        inline String GetVersionString() const
        {
            return m_ApplicationSpecs.VersionString;
        }

    public:
        [[nodiscard]]
        inline SharedPtr<IMonitor> GetMonitor(Index index) const
        {
            return m_Monitors[index];
        }

        [[nodiscard]]
        inline Usize GetMonitorCount() const
        {
            return m_Monitors.Size();
        }

        [[nodiscard]]
        inline SharedPtr<IMonitor> GetPrimaryMonitor() const
        {
            auto it = Algorithms::FindIf(m_Monitors.GetBegin(), m_Monitors.GetEnd(),
                [](const SharedPtr<IMonitor>& monitor) -> bool { return monitor->IsPrimaryMonitor(); });

            KITSUNE_ASSERT(it != m_Monitors.GetEnd(), "No primary monitor was found.");
            return *it;
        }

        [[nodiscard]]
        inline SharedPtr<IWindow> GetWindow() const { return m_PrimaryWindow; }

    public:
        inline void Quit(int exitCode) { Process::Exit(exitCode); }

        inline bool IsExitRequested() const
        {
            return Process::IsExitRequested();
        }

        inline int GetExitCode() const
        {
            return Process::GetExitCode();
        }

    private:
        void PlatformUpdate();
        SharedPtr<IWindow> MakePlatformWindow(const WindowProperties& props);

        Array<SharedPtr<IMonitor>> RetrieveMonitors();

    private:
        static void VerifyApplicationSpecs(const ApplicationSpecs& specs);
        static void VerifyWindowProperties(const WindowProperties& windowProps);

    private:
        ApplicationSpecs m_ApplicationSpecs;

        SharedPtr<IWindow> m_PrimaryWindow;
        Array<SharedPtr<IMonitor>> m_Monitors;
    };

    // Should be defined in client code.
    extern Kitsune::Application* CreateApplication(const Kitsune::CommandLineArguments&);
}

KITSUNE_POP_COMPILER_WARNINGS()
