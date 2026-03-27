#pragma once

#include "Foundation/Logging/Logger.h"
#include "Foundation/Threading/Mutex.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Containers/Array.h"
#include "Foundation/Diagnostics/Backtrace.h"

#include "Application/Application.h"
#include "Application/CommandLineArguments.h"

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
        inline const Backtrace& GetExceptionBacktrace() const
        {
            return m_ExceptionBacktrace;
        }

        [[nodiscard]]
        inline const CommandLineArguments& GetCommandLineArguments() const
        {
            return m_CommandLineArguments;
        }

    public:
        template<std::derived_from<Logger> LoggerT, typename... Args>
        inline void RegisterLogger(Args&&... args)
        {
            m_Loggers.PushBack(MakeScoped<LoggerT>(Forward<Args>(args)...));
        }

        [[nodiscard]]
        inline Array<ScopedPtr<Logger>>& GetLoggers()
        {
            return m_Loggers;
        }

        [[nodiscard]]
        inline const Array<ScopedPtr<Logger>>& GetLoggers() const
        {
            return m_Loggers;
        }

    public:
        // Should only be called in the Exception class's constructor.
        inline void CaptureExceptionBacktrace()
        {
            // If there are multiple threads calling this, we don't want the
            // backtrace to keep getting overridden.
            if (!m_ExceptionMutex.TryAcquire())
                return;

            m_ExceptionBacktrace = Backtrace::Capture(1);
            m_ExceptionMutex.Release();
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
        static EngineLoop* s_Instance;

    public:
        CommandLineArguments m_CommandLineArguments;
        Array<ScopedPtr<Logger>> m_Loggers;

        Int32 m_ExitCode = 0;
        bool m_ExitRequested = false;

        Backtrace m_ExceptionBacktrace;
        Mutex m_ExceptionMutex;

        Application* m_Application = nullptr;
    };
}
