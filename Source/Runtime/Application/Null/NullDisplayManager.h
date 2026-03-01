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
        inline NullDisplayManager(ScopedPtr<NullScreen>&& screen)
            : m_PrimaryScreen(Move(screen))
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
        inline WindowHandle MakeWindow(const WindowSpecifications& specs) override
        {
            m_Windows.PushBack(MakeScoped<NullWindow>(specs.Size, specs.Position, specs.Title,
                                                      specs.Mode, specs.Flags));

            return m_Windows.Back().Get();
        }

        inline void DestroyWindow(WindowHandle handle) override
        {
            auto iter = Algorithms::FindIf(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                [&](const ScopedPtr<NullWindow>& window) -> bool
                {
                    return (window.Get() == handle);
                });

            if (iter == m_Windows.GetEnd())
            {
                throw SystemException("Tried to destroy a window which was not created by "
                                      "this display manager.");
            }

            m_Windows.Remove(iter);
        }

        inline WindowHandle GetPrimaryWindow() const override
        {
            if (m_Windows.IsEmpty())
                return nullptr;

            return m_Windows[0].Get();
        }

    private:
        ScopedPtr<NullScreen> m_PrimaryScreen;
        Array<ScopedPtr<NullWindow>> m_Windows;
    };
}
