#pragma once

#include "ApplicationCore/IMonitor.h"

namespace Kitsune
{
    class NullMonitor : public IMonitor
    {
    public:
        inline NullMonitor()
            : NullMonitor(VideoMode(32, { 1920, 1080 }, 60), true)
        {
        }

        inline NullMonitor(const VideoMode& videoMode, bool primary)
            : m_VideoMode(videoMode), m_Primary(primary)
        {
        }

        ~NullMonitor() = default;

    public:
        inline Vector2<Int32> GetVirtualPosition() const { return { 0, 0 }; }
        inline String GetName() const { return "Kitsune_NullMonitor"; }

        inline VideoMode GetVideoMode() const { return m_VideoMode; }
        inline void SetVideoMode(const VideoMode& videoMode)
        {
            m_VideoMode = videoMode;
        }

        inline bool IsPrimaryMonitor() const { return m_Primary; }

    private:
        VideoMode m_VideoMode;
        bool m_Primary;
    };
}
