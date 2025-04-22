#pragma once

#include "Foundation/Diagnostics/Assert.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/CoreApplication.h"

#include "ApplicationCore/CommandLineArguments.h"

KITSUNE_PUSH_COMPILER_WARNINGS()

KITSUNE_IGNORE_MSVC_WARNING(4100)       // 'XXX': Unreferenced format parameter (in virtual functions)
KITSUNE_IGNORE_CLANG_WARNING(-Wunused-parameter)

namespace Kitsune
{
    class Application : private CoreApplication
    {
    public:
        KITSUNE_API_ Application(const ApplicationSpecs& specs);
        KITSUNE_API_ virtual ~Application();

    public:
        KITSUNE_API_ void Update();

    public:
        virtual void OnUpdate() { /* ... */ }

        virtual void OnWindowResize(const Vector2<Uint32>& size) { /* ... */ }
        virtual void OnWindowMove(const Vector2<Int32>& pos) { /* ... */ }

        virtual void OnWindowMaximize() { /* ... */ }
        virtual void OnWindowMinimize() { /* ... */ }

    public:
        inline bool IsExitRequested() const
        {
            return m_ExitRequested;
        }

        [[nodiscard]]
        inline int GetExitCode() const
        {
            return m_ExitCode;
        }

        inline void Exit(int exitCode)
        {
            CoreApplication::PlatformExit(exitCode);
        }

        [[noreturn]]
        inline void ForceExit(int exitCode)
        {
            CoreApplication::PlatformForceExit(exitCode);
        }

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
        inline IMonitor* GetMonitor(Index index) const
        {
            return m_Monitors[index].Get();
        }

        [[nodiscard]]
        inline Usize GetMonitorCount() const
        {
            return m_Monitors.Size();
        }

        [[nodiscard]]
        inline IMonitor* GetPrimaryMonitor() const
        {
            auto it = Algorithms::FindIf(m_Monitors.GetBegin(), m_Monitors.GetEnd(),
                [](const ScopedPtr<IMonitor>& monitor) -> bool { return monitor->IsPrimaryMonitor(); });

            KITSUNE_ASSERT(it != m_Monitors.GetEnd(), "No primary monitor was found.");
            return it->Get();
        }

        [[nodiscard]]
        inline IWindow* GetWindow() const { return m_PrimaryWindow.Get(); }

    public:
        [[nodiscard]] static inline Application& GetInstance()
        {
            KITSUNE_ASSERT(s_Instance != nullptr,
                "No instance of the application has been created.");

            return *s_Instance;
        }

    private:
        KITSUNE_API_ static void VerifyApplicationSpecs(const ApplicationSpecs& specs);

    private:
        KITSUNE_API_ static Application* s_Instance;
    };

    // Should be defined in client code.
    extern Kitsune::Application* CreateApplication(const Kitsune::CommandLineArguments&);
}

KITSUNE_POP_COMPILER_WARNINGS()
