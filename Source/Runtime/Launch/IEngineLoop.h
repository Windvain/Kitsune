#pragma once

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class EngineLoopNotification
    {
        Crash,
        OutOfMemory
    };

    // Made abstract so that editor code won't have to be mixed in with
    // engine initialization code.
    class IEngineLoop : public NonCopyable
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
