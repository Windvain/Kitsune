#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Memory/ScopedPtr.h"

namespace Kitsune
{
    // An application's platform-specific behaviour.
    class IPlatformApplication
    {
    public:
        virtual ~IPlatformApplication() { /* ... */ }

    public:
        virtual void Exit(int exitCode) = 0;
        virtual void ForceExit(int exitCode) = 0;

        virtual bool IsExitRequested() const = 0;
        virtual int GetExitCode() const = 0;

    public:
        // To be implemented in the platform-specific implementation.
        KITSUNE_API_ static ScopedPtr<IPlatformApplication> CreateApplicationImpl();
    };
}
