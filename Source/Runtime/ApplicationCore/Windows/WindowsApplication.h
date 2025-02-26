#pragma once

#include <Windows.h>
#include "ApplicationCore/IPlatformApplication.h"

namespace Kitsune
{
    class WindowsApplication : public IPlatformApplication
    {
    public:
        KITSUNE_API_ WindowsApplication();
        KITSUNE_API_ ~WindowsApplication();

    public:
        KITSUNE_API_ void Exit(int exitCode) override;
        KITSUNE_API_ void ForceExit(int exitCode) override;

        inline bool IsExitRequested() const override { return m_ExitRequested; }
        inline int GetExitCode() const override { return m_ExitCode; }

    public:
        KITSUNE_API_ void PollEvents() override;

    public:
        KITSUNE_API_ SharedPtr<IMonitor> AllocatePrimaryMonitor() override;

    private:
        bool m_ExitRequested = false;
        int m_ExitCode = 0;
    };
}
