#pragma once

#include "Display/WindowManager.h"

#include "Foundation/Memory/ScopedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Algorithms/Contains.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    class NullWindowManager : public WindowManager
    {
    private:
        // Forward declare for function declarations.
        struct NullWindow;

    public:
        inline void Update(double delta) override
        {
            KITSUNE_UNUSED(delta);
        }

    public:
        [[nodiscard]]
        inline WindowID CreateWindow(const WindowConfigurations& configs) override
        {
            NullWindow window = {
                .Title = configs.Title,
                .Visible = true,
                .Size = configs.Size,
                .Position = configs.Position,
                .Flags = configs.Flags,
                .State = configs.State
            };

            m_Windows.PushBack(MakeScoped<NullWindow>(window));
            return reinterpret_cast<WindowID>(m_Windows.Back().Get());
        }

        inline void DestroyWindow(WindowID windowID) override
        {
            auto iter = Algorithms::Find(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowID));

            if (iter == m_Windows.GetEnd())
                throw InvalidArgumentException("The window is invalid.");

            m_Windows.RemoveUnsorted(iter);
        }

        inline bool IsWindowClosed(WindowID windowID) const override
        {
            return !Algorithms::Contains(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowID));
        }

    public:
        [[nodiscard]]
        inline Vector2<Uint32> GetWindowSize(WindowID windowID) const override
        {
            return GetWindow(windowID)->Size;
        }

        [[nodiscard]]
        inline Vector2<Int32> GetWindowPosition(WindowID windowID) const override
        {
            return GetWindow(windowID)->Position;
        }

        [[nodiscard]]
        inline String GetWindowTitle(WindowID windowID) const override
        {
            return GetWindow(windowID)->Title;
        }

        [[nodiscard]]
        inline WindowState GetWindowState(WindowID windowID) const override
        {
            return GetWindow(windowID)->State;
        }

        [[nodiscard]]
        inline bool IsWindowVisible(WindowID windowID) const override
        {
            return GetWindow(windowID)->Visible;
        }

    public:
        inline void SetWindowSize(WindowID windowID, const Vector2<Uint32>& size) override
        {
            auto& window = GetWindow(windowID);
            if (window->State != WindowState::Windowed)
                return;

            if (!bool(window->Flags & WindowCreationFlags::ResizeEnabled))
                return;

            GetWindow(windowID)->Size = size;
        }

        inline void SetWindowPosition(
            WindowID windowID, const Vector2<Int32>& position) override
        {
            auto& window = GetWindow(windowID);
            if (window->State != WindowState::Windowed)
                return;

            GetWindow(windowID)->Position = position;
        }

        inline void SetWindowTitle(WindowID windowID, StringView title) override
        {
            GetWindow(windowID)->Title = title;
        }

        inline void SetWindowState(WindowID windowID, WindowState state) override
        {
            if (!IsWindowVisible(windowID))
                return;

            GetWindow(windowID)->State = state;
        }

        inline void SetWindowVisibility(WindowID windowID, bool visible) override
        {
            GetWindow(windowID)->Visible = visible;
        }

    private:
        inline ScopedPtr<NullWindow>& GetWindow(WindowID windowID)
        {
            auto iter = Algorithms::Find(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowID));

            if (iter == m_Windows.GetEnd())
                throw InvalidArgumentException("The window ID is invalid.");

            return *iter;
        }

        inline const ScopedPtr<NullWindow>& GetWindow(WindowID windowID) const
        {
            auto iter = Algorithms::Find(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowID));

            if (iter == m_Windows.GetEnd())
                throw InvalidArgumentException("The window ID is invalid.");

            return *iter;
        }

    private:
        struct NullWindow
        {
            String Title;
            bool Visible;

            Vector2<Uint32> Size;
            Vector2<Int32> Position;

            WindowCreationFlags Flags;
            WindowState State;
        };

        Array<ScopedPtr<NullWindow>> m_Windows;
    };
}
