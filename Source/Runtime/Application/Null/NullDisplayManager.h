#pragma once

#include "Application/DisplayManager.h"

#include "Application/Null/NullScreen.h"
#include "Application/Null/NullWindow.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Diagnostics/SystemException.h"

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

        ~NullDisplayManager() = default;

    public:
        inline void Update() override
        {
        }

        inline ScreenHandle GetPrimaryScreen() const override
        {
            return m_PrimaryScreen.Get();
        }

        inline Array<ScreenHandle> GetScreens() const override
        {
            return { m_PrimaryScreen.Get() };
        }

    public:
        inline WindowHandle GetPrimaryWindow() const override
        {
            return m_PrimaryWindow.Get();
        }

    private:
        inline static ScopedPtr<NullWindow> CreateNullWindow_(
            const WindowSpecifications& specs)
        {
            return MakeScoped<NullWindow>(
                specs.Size,
                specs.Position,
                specs.Title,
                specs.Mode,
                specs.Flags);
        }

    private:
        ScopedPtr<NullScreen> m_PrimaryScreen;
        ScopedPtr<NullWindow> m_PrimaryWindow;
    };
}
