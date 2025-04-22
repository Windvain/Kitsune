#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "ApplicationCore/VideoMode.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    class IMonitor : public NonCopyable
    {
    public:
        virtual ~IMonitor() { /* ... */ }

    public:
        virtual Vector2<Int32> GetVirtualPosition() const = 0;
        virtual String GetName() const = 0;

        virtual VideoMode GetVideoMode() const = 0;
        virtual void SetVideoMode(const VideoMode& videoMode) = 0;

        virtual bool IsPrimaryMonitor() const = 0;
    };
}
