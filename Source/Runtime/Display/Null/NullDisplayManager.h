#pragma once

#include "Display/DisplayManager.h"

#include "Display/Null/NullWindow.h"
#include "Display/Null/NullDisplay.h"

namespace Kitsune
{
    class NullDisplayManager : public DisplayManager
    {
    public:
        inline NullDisplayManager(const Vector2<Uint32>& size, Uint32 refreshRate,
                                  float scaling)
            : m_Display(MakeShared<NullDisplay>(size, refreshRate, scaling))
        {
        }

    public:
        inline void Update() override
        {
        }

    public:
        [[nodiscard]]
        inline ScopedPtr<Window> CreateWindow(
            const WindowConfigurations& configurations) override
        {
            return MakeScoped<NullWindow>(configurations);
        }

    public:
        [[nodiscard]]
        inline Array<SharedPtr<Display>> GetDisplays() const override
        {
            return { m_Display };
        }

        [[nodiscard]]
        inline SharedPtr<Display> GetMainDisplay() const override
        {
            return m_Display;
        }

    private:
        SharedPtr<NullDisplay> m_Display;
    };
}
