#pragma once

#include <Windows.h>
#undef CreateWindow

#include "Display/DisplayManager.h"

#include "Display/Windows/WindowsWindow.h"
#include "Display/Windows/WindowsDisplay.h"

namespace Kitsune
{
    class WindowsDisplayManager : public DisplayManager
    {
    public:
        KITSUNE_API explicit WindowsDisplayManager(WideStringView className);
        KITSUNE_API ~WindowsDisplayManager() override;

    public:
        KITSUNE_API void Update() override;

    public:
        [[nodiscard]]
        inline ScopedPtr<Window> CreateWindow(
            const WindowConfigurations& configurations) override
        {
            return MakeScoped<WindowsWindow>(configurations);
        }

    public:
        [[nodiscard]]
        inline Array<SharedPtr<Display>> GetDisplays() const override
        {
            Array<SharedPtr<Display>> displays;
            for (const auto& display : m_Displays)
                displays.PushBack(display);

            return displays;
        }

        [[nodiscard]]
        inline SharedPtr<Display> GetMainDisplay() const override
        {
            if (m_Displays.IsEmpty())
                return nullptr;

            return m_Displays[0];
        }

    public:
        [[nodiscard]]
        inline const wchar_t* GetWindowClassName()
        {
            return m_WindowClassName.Raw();
        }

    public:
        inline static WindowsDisplayManager* GetInstance()
        {
            return s_Instance;
        }

    private:
        void UpdateDisplays();

        static LRESULT WindowProcedure(
            HWND handle, UINT message, WPARAM wparam, LPARAM lparam);

    private:
        static WindowsDisplayManager* s_Instance;

    private:
        WideString m_WindowClassName;

        Array<SharedPtr<WindowsDisplay>> m_Displays;
        Array<SharedPtr<WindowsDisplay>> m_OldDisplays;
    };
}
