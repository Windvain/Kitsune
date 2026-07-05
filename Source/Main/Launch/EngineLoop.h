#pragma once

#include "Core/Application.h"
#include "Core/CommandLineArguments.h"

#include "Display/DisplayManager.h"
#include "Foundation/Logging/Logger.h"

namespace Kitsune
{
    // Contains early engine initialization code.
    class EngineLoop : public NonCopyable
    {
    public:
        EngineLoop();
        ~EngineLoop();

    public:
        void Initialize(int argc, char** argv);
        void Run();

        int Shutdown();

    public:
        void Exit(int exitCode);
        [[noreturn]] void ForceExit(int exitCode);

    public:
        [[nodiscard]]
        inline const CommandLineArguments& GetCommandLineArguments() const
        {
            return m_CommandLineArguments;
        }

    public:
        [[nodiscard]]
        inline static String GetEngineVersion()
        {
            return KITSUNE_STRINGIFY(KITSUNE_VERSION_MAJOR) "."
                   KITSUNE_STRINGIFY(KITSUNE_VERSION_MINOR) "."
                   KITSUNE_STRINGIFY(KITSUNE_VERSION_PATCH);
        }

    public:
        [[nodiscard]]
        inline static EngineLoop* GetInstance()
        {
            return s_Instance;
        }

    private:
        [[nodiscard]] static bool CPUSupportsSIMDRequirements();

    private:
        static EngineLoop* s_Instance;

    public:
        CommandLineArguments m_CommandLineArguments;
        Logger* m_Logger = nullptr;

        DisplayManager* m_DisplayManager = nullptr;

        Int32 m_ExitCode = 0;
        bool m_ExitRequested = false;

        Application* m_Application = nullptr;
    };
}
