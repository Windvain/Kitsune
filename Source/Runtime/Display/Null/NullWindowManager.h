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
        inline WindowId CreateWindow(const WindowConfigurations& configs) override
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
            return reinterpret_cast<WindowId>(m_Windows.Back().Get());
        }

        inline void DestroyWindow(WindowId windowId) override
        {
            auto iter = Algorithms::Find(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowId));

            if (iter == m_Windows.GetEnd())
                throw InvalidArgumentException("The window is invalid.");

            m_Windows.RemoveUnsorted(iter);
        }

        inline bool IsWindowClosed(WindowId windowId) const override
        {
            return !Algorithms::Contains(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowId));
        }

    public:
        [[nodiscard]]
        inline Vector2<Uint32> GetWindowSize(WindowId windowId) const override
        {
            return GetWindow(windowId)->Size;
        }

        [[nodiscard]]
        inline Vector2<Int32> GetWindowPosition(WindowId windowId) const override
        {
            return GetWindow(windowId)->Position;
        }

        [[nodiscard]]
        inline String GetWindowTitle(WindowId windowId) const override
        {
            return GetWindow(windowId)->Title;
        }

        [[nodiscard]]
        inline WindowState GetWindowState(WindowId windowId) const override
        {
            return GetWindow(windowId)->State;
        }

        [[nodiscard]]
        inline bool IsWindowVisible(WindowId windowId) const override
        {
            return GetWindow(windowId)->Visible;
        }

    public:
        inline void SetWindowSize(WindowId windowId, const Vector2<Uint32>& size) override
        {
            auto& window = GetWindow(windowId);
            if (window->State != WindowState::Windowed)
                return;

            if (!bool(window->Flags & WindowCreationFlags::ResizeEnabled))
                return;

            GetWindow(windowId)->Size = size;
        }

        inline void SetWindowPosition(
            WindowId windowId, const Vector2<Int32>& position) override
        {
            auto& window = GetWindow(windowId);
            if (window->State != WindowState::Windowed)
                return;

            GetWindow(windowId)->Position = position;
        }

        inline void SetWindowTitle(WindowId windowId, StringView title) override
        {
            GetWindow(windowId)->Title = title;
        }

        inline void SetWindowState(WindowId windowId, WindowState state) override
        {
            if (!IsWindowVisible(windowId))
                return;

            GetWindow(windowId)->State = state;
        }

        inline void SetWindowVisibility(WindowId windowId, bool visible) override
        {
            GetWindow(windowId)->Visible = visible;
        }

    private:
        inline ScopedPtr<NullWindow>& GetWindow(WindowId windowId)
        {
            auto iter = Algorithms::Find(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowId));

            if (iter == m_Windows.GetEnd())
                throw InvalidArgumentException("The window ID is invalid.");

            return *iter;
        }

        inline const ScopedPtr<NullWindow>& GetWindow(WindowId windowId) const
        {
            auto iter = Algorithms::Find(
                m_Windows.GetBegin(), m_Windows.GetEnd(),
                reinterpret_cast<NullWindow*>(windowId));

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
