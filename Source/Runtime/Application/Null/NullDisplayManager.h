#pragma once

#include "Application/DisplayManager.h"
#include "Application/Null/NullScreen.h"

namespace Kitsune
{
    class NullDisplayManager : public DisplayManager
    {
    public:
        inline NullDisplayManager()
            : m_PrimaryScreen({ 1920, 1080 }, { 0, 0 }, 60, 96,
                              ScreenOrientation::Default)
        {
        }

        ~NullDisplayManager() = default;

    public:
        inline void Update() override
        {
        }

    private:
        NullScreen m_PrimaryScreen;
    };
}
