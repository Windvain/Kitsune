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
        inline Array<DisplayID> GetDisplays() const override
        {
            Array<DisplayID> displayIDs;
            for (const ScopedPtr<WindowsDisplay>& display : m_Displays)
                displayIDs.PushBack(static_cast<void*>(display.Get()));

            return displayIDs;
        }

        [[nodiscard]]
        inline DisplayID GetMainDisplay() const override
        {
            // During the updating of all of our displays, we will insert the main
            // display at the beginning.
            DisplayID displayID = nullptr;
            if (!m_Displays.IsEmpty())
                displayID = static_cast<void*>(m_Displays[0].Get());

            return displayID;
        }

        [[nodiscard]]
        inline Usize GetDisplayCount() const override
        {
            return m_Displays.Size();
        }

    public:
        [[nodiscard]]
        DisplayInformation GetDisplayInformation(DisplayID displayID) const override;

        [[nodiscard]]
        inline bool IsDisplayConnected(DisplayID displayID) const override
        {
            return Algorithms::Contains(
                m_Displays.GetBegin(), m_Displays.GetEnd(), displayID);
        }

    public:
        void SetDisplayOrientation(
            DisplayID displayID,
            DisplayOrientation orientation) const override;

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
