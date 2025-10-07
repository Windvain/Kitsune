#pragma once

#include "Application/CommandLineArguments.h"

namespace Kitsune
{
    enum class EngineLoopNotification
    {
        Crash,
        OutOfMemory
    };

    class IEngineLoop
    {
    public:
        virtual ~IEngineLoop() { /* ... */ }

        virtual bool Initialize(int argc, char** argv) = 0;
        virtual int Run() = 0;

        virtual void Shutdown() = 0;

    public:
        virtual void Notify(EngineLoopNotification notification) = 0;
    };
}
