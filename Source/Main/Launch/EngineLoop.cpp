#include "Launch/EngineLoop.h"

#include <cstdlib>
#include "Foundation/Algorithms/Contains.h"
#include "Foundation/Logging/ConsoleLogSink.h"

#include "Foundation/Diagnostics/LogicException.h"
#include "Foundation/Utilities/SystemInformation.h"

namespace Kitsune
{
    EngineLoop* EngineLoop::s_Instance = nullptr;

    EngineLoop::EngineLoop()
    {
        if (s_Instance != nullptr)
        {
            throw LogicException(
                "An instance of the engine loop has already been created.");
        }

        s_Instance = this;
    }

    EngineLoop::~EngineLoop()
    {
        s_Instance = nullptr;
    }

    void EngineLoop::Initialize(int argc, char** argv)
    {
        m_CommandLineArguments = CommandLineArguments(argc, argv);
        m_Logger = Memory::New<Logger>();

#if !defined(KITSUNE_BUILD_PRODUCTION)
        m_Logger->RegisterSink(MakeScoped<ConsoleLogSink>());
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

        // TODO: Read this from a config file?
        String displayServer;
#if defined(KITSUNE_OS_WINDOWS)
        displayServer = "Windows";
#endif

        m_DisplayManager = DisplayManager::Initialize({
            .DisplayServer = displayServer,
            .NullDisplay = {
                .Size = { 1920, 1080 },
                .RefreshRate = 60,
                .Orientation = DisplayOrientation::Default
            }
        });

        m_WindowManager = WindowManager::Initialize(displayServer);

        KITSUNE_ENGINE_INFO(
            Launch,
            "Kitsune Engine initialization step ran successfully. Calling the "
            "application's constructor.");

        m_Application = CreateApplication(m_CommandLineArguments);
        if (m_Application == nullptr)
            Exit(EXIT_FAILURE);
    }

    // NOTE: This is done just to suppress warnings. Remove the NOLINT comment once
    // Run() is made non-const.
    // NOLINTNEXTLINE(readability-make-member-function-const)
    void EngineLoop::Run()
    {
        KITSUNE_ENGINE_INFO(
            Launch,
            "Running the application, application callbacks will start to be called "
            "from here on!");

        while (!m_ExitRequested)
        {
            m_DisplayManager->Update(/* Temp */ 0);
            m_WindowManager->Update(/* Temp */ 0);

            m_Application->OnUpdate(/* Temp */ 0);
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
}
