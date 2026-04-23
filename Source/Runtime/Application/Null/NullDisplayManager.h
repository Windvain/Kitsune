#pragma once

#include "Application/DisplayManager.h"

#include "Application/Null/NullScreen.h"
#include "Application/Null/NullWindow.h"

#include "Foundation/Memory/ScopedPtr.h"

namespace Kitsune
{
    class NullDisplayManager : public DisplayManager
    {
    public:
        inline NullDisplayManager(ScopedPtr<NullScreen>&& screen,
                                  const DisplayManagerSpecifications& specs)
            : m_PrimaryScreen(Move(screen)),
              m_PrimaryWindow(CreateNullWindow_(specs.PrimaryWindowSpecs))
        {
        }

        ~NullDisplayManager() override = default;

    public:
        inline void Update() override
        {
        }

        [[nodiscard]]
        inline Screen* GetPrimaryScreen() const override
        {
            return m_PrimaryScreen.Get();
        }

        [[nodiscard]]
        inline Array<Screen*> GetScreens() const override
        {
            return { m_PrimaryScreen.Get() };
        }

    public:
        [[nodiscard]]
        inline Window* GetPrimaryWindow() const override
        {
            return m_PrimaryWindow.Get();
        }

    private:
        [[nodiscard]]
        inline static ScopedPtr<NullWindow> CreateNullWindow_(
            const WindowSpecifications& specs)
        {
            return MakeScoped<NullWindow>(
                specs.Size, specs.Position,
                specs.Title, specs.Mode, specs.Flags);
        }

    private:
        ScopedPtr<NullScreen> m_PrimaryScreen;
        ScopedPtr<NullWindow> m_PrimaryWindow;
    };
}
