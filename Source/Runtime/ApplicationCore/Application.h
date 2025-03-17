#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Memory/ScopedPtr.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/IPlatformApplication.h"
#include "ApplicationCore/CommandLineArguments.h"

KITSUNE_PUSH_COMPILER_WARNINGS()

KITSUNE_IGNORE_MSVC_WARNING(4100)       // 'XXX': Unreferenced format parameter (in virtual functions)
KITSUNE_IGNORE_CLANG_WARNING(-Wunused-parameter)

namespace Kitsune
{
    struct ApplicationSpecs
    {
        String Name;
        Vector2<Uint32> ViewportSize = { 640, 480 };

        Vector2<Int32> WindowPosition;
        WindowPositionHint WindowPositionHint = WindowPositionHint::UsePosition;
        WindowState WindowState = WindowState::Floating;

        bool IsConsoleApp = false;
    };

    class Application
    {
    public:
        KITSUNE_API_ Application(ApplicationSpecs specs);
        KITSUNE_API_ virtual ~Application();

    public:
        KITSUNE_API_ void Update();

    public:
        inline bool IsExitRequested() const { return m_PlatformImpl->IsExitRequested(); }

        inline void Exit(int exitCode)      { return m_PlatformImpl->Exit(exitCode); }
        inline void ForceExit(int exitCode) { return m_PlatformImpl->ForceExit(exitCode); }

        inline int GetExitCode()      const { return m_PlatformImpl->GetExitCode(); }

    public:
        inline SharedPtr<IWindow>  GetPrimaryWindow()  const { return m_PrimaryWindow; }
        inline SharedPtr<IMonitor> GetPrimaryMonitor() const { return m_PrimaryMonitor; }

    public:
        virtual void OnUpdate() { /* ... */ }

        virtual void OnWindowResize(IWindow& window, const Vector2<Uint32>& size) { /* ... */ }
        virtual void OnWindowMove(IWindow& window, const Vector2<Int32>& pos) { /* ... */ }

        virtual void OnWindowMaximize(IWindow& window) { /* ... */ }
        virtual void OnWindowMinimize(IWindow& window) { /* ... */ }

    public:
        static inline Application* GetInstance() { return s_Instance; }

    private:
        KITSUNE_API_ static Application* s_Instance;

    private:
        ApplicationSpecs m_ApplicationSpecs;
        ScopedPtr<IPlatformApplication> m_PlatformImpl;

        SharedPtr<IWindow> m_PrimaryWindow;
        SharedPtr<IMonitor> m_PrimaryMonitor;
    };

    // Should be defined in client code.
    extern Kitsune::Application* CreateApplication(const Kitsune::CommandLineArguments&);
}

KITSUNE_POP_COMPILER_WARNINGS()
