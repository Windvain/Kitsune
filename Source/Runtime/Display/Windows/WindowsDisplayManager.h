#pragma once

#include <Windows.h>
#include "Display/DisplayManager.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Algorithms/Contains.h"

namespace Kitsune
{
    class WindowsDisplayManager : public DisplayManager
    {
    public:
        inline WindowsDisplayManager()
        {
            Update(0);
        }

    public:
        void Update(double delta) override;

    public:
        [[nodiscard]]
        inline Array<DisplayId> GetDisplays() const override
        {
            Array<DisplayId> displayIds;
            for (const ScopedPtr<WindowsDisplay>& display : m_Displays)
                displayIds.PushBack(static_cast<void*>(display.Get()));

            return displayIds;
        }

        [[nodiscard]]
        inline DisplayId GetMainDisplay() const override
        {
            // During the updating of all of our displays, we will insert the main
            // display at the beginning.
            DisplayId displayId = nullptr;
            if (!m_Displays.IsEmpty())
                displayId = static_cast<void*>(m_Displays[0].Get());

            return displayId;
        }

        [[nodiscard]]
        inline Usize GetDisplayCount() const override
        {
            return m_Displays.Size();
        }

    public:
        [[nodiscard]]
        DisplayInformation GetDisplayInformation(DisplayId displayId) const override;

        [[nodiscard]]
        inline bool IsDisplayConnected(DisplayId displayId) const override
        {
            return Algorithms::Contains(
                m_Displays.GetBegin(), m_Displays.GetEnd(), displayId);
        }

    public:
        void SetDisplayOrientation(
            DisplayId displayId,
            DisplayOrientation orientation) override;

    private:
        static BOOL MonitorEnumerationProc(
            HMONITOR monitor, HDC deviceContext, LPRECT rect, LPARAM lparam);

    private:
        struct WindowsDisplay
        {
            wchar_t DeviceName[32];
        };

        Array<ScopedPtr<WindowsDisplay>> m_Displays;
        Array<ScopedPtr<WindowsDisplay>> m_OldDisplays;
    };
}
