#include "Core/EngineLoop.h"

#include <cstdlib>
#include "Foundation/Algorithms/Contains.h"

#include "Foundation/Logging/FileLogSink.h"
#include "Foundation/Logging/ConsoleLogSink.h"

#include "Foundation/Utilities/SystemInformation.h"
#include "Foundation/Diagnostics/SingletonException.h"

#include "Foundation/Filesystem/Filesystem.h"
#include "Foundation/Filesystem/ExecutablePath.h"
#include "Foundation/Filesystem/CurrentDirectory.h"

namespace Kitsune
{
    EngineLoop* EngineLoop::s_Instance = nullptr;

    EngineLoop::EngineLoop()
    {
        if (s_Instance != nullptr)
            throw SingletonException();

        s_Instance = this;
    }

    EngineLoop::~EngineLoop()
    {
        s_Instance = nullptr;
    }

    void EngineLoop::Initialize(int argc, char** argv)
    {
        InitializeDirectoryPaths();

        m_CommandLineArguments = CommandLineArguments(argc, argv);
        m_Logger = Memory::New<Logger>();

#if !defined(KITSUNE_BUILD_PRODUCTION)
        m_Logger->RegisterSink(MakeScoped<ConsoleLogSink>());
        m_Logger->RegisterSink(
            MakeScoped<FileLogSink>(m_LogDirectory / "Engine-dy-mn-yr.log"));
#endif

        KITSUNE_ENGINE_INFO_FORMAT(
            Launch,
            "Initializing Kitsune Engine {0}. "
            "For the source code, visit https://github.com/Windvain/Kitsune",
            GetEngineVersion());

#if defined(KITSUNE_ENABLE_SIMD_OPTIMIZATIONS)
        if (!CPUSupportsSIMDRequirements())
        {
            ForceExit(EXIT_FAILURE);
            return;
        }
#endif

        KITSUNE_ENGINE_INFO_FORMAT(
            Launch,
            "Setting the current directory from {0} to {1}.",
            Filesystem::GetCurrentDirectory(),
            m_ApplicationDirectory);

        SetCurrentDirectory(m_ApplicationDirectory);

        KITSUNE_ENGINE_INFO(
            Launch,
            "Kitsune Engine initialization step ran successfully.");
    }

    // NOTE: This is done just to suppress warnings. Remove the NOLINT comment once
    // Run() is made non-const.
    // NOLINTNEXTLINE(readability-make-member-function-const)
    void EngineLoop::Run(Application* application)
    {
        KITSUNE_ENGINE_INFO(
            Launch,
            "Running the application, application callbacks will start to be called "
            "from here on!");

        if (application == nullptr)
            Exit(EXIT_FAILURE);

        m_Application = application;
        while (!m_ExitRequested)
        {
            const auto& window = m_Application->GetWindow();
            if (!window->IsOpen())
                Exit(0);

            m_Application->Update(/* Temp */ 0);
        }
    }

    int EngineLoop::Shutdown()
    {
        KITSUNE_ENGINE_INFO(Launch, "Kitsune Engine shutting down, goodbye!");

        if (m_Application != nullptr)
        {
            Memory::Delete(m_Application);
            m_Application = nullptr;
        }

        Memory::Delete(m_Logger);
        m_Logger = nullptr;

        return m_ExitCode;
    }

    bool EngineLoop::CPUSupportsSIMDRequirements()
    {
        KITSUNE_ENGINE_INFO(Launch, "Checking SIMD support...");

        CPUFeatures features = SystemInformation::GetCPUFeatures();
        Array<bool> supported;

        const auto LogSupportMessage = [](bool supported, const char* featureName)
        {
            KITSUNE_ENGINE_LOG_FORMAT(
                Launch,
                supported ? LogSeverity::Info : LogSeverity::Fatal,
                SourceLocation(),
                "\tSupports {0}: {1}", featureName, supported);
        };

#if defined(KITSUNE_ARCH_X86)
        {
            auto supportsAVX2 = bool(features & CPUFeatures::AVX2);
            LogSupportMessage(supportsAVX2, "AVX2");

            supported.PushBack(supportsAVX2);
        }
#endif

        KITSUNE_ENGINE_INFO(Launch, "Finished SIMD checks.");
        return !Algorithms::Contains(supported.GetBegin(), supported.GetEnd(), false);
    }

    void EngineLoop::InitializeDirectoryPaths()
    {
        m_ApplicationDirectory = Filesystem::GetExecutablePath().GetParentPath();
        m_LogDirectory = m_ApplicationDirectory / "Logs";

        KITSUNE_UNUSED(CreateDirectory(m_LogDirectory));
    }
}
