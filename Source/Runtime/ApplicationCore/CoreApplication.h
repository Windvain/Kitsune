#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Memory/ScopedPtr.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/IMonitor.h"
#include "ApplicationCore/WindowException.h"

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

    class CoreApplication
    {
    protected:
        KITSUNE_API_ CoreApplication(const ApplicationSpecs& specs);
        KITSUNE_API_ ~CoreApplication();

    protected:
        KITSUNE_API_ void PlatformExit(int exitCode);
        KITSUNE_API_ void PlatformForceExit(int exitCode);

    protected:
        KITSUNE_API_ void PlatformUpdate();

        [[nodiscard]]
        KITSUNE_API_ void MakeWindow(const WindowProperties& windowProps);

    private:
        inline void VerifyWindowProperties(const WindowProperties& windowProps)
        {
            if (windowProps.Size == Vector2<Uint32>())
                throw WindowException("Cannot create a window with a size of [0, 0].");
        }

    protected:
        ApplicationSpecs m_ApplicationSpecs;

        int m_ExitCode = 0;
        bool m_ExitRequested = false;

        Array<ScopedPtr<IMonitor>> m_Monitors;
        ScopedPtr<IWindow> m_PrimaryWindow;
    };
}
