#pragma once

#include "Display/Window.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    class NullWindow : public Window
    {
    public:
        inline explicit NullWindow(const WindowConfigurations& configurations)
        {
            Open(configurations);
        }

        inline ~NullWindow() override
        {
            if (IsOpen())
                Close();
        }

    public:
        inline void Open(const WindowConfigurations& configurations) override
        {
            if (m_IsOpen)
            {
                throw SystemException(
                    "Failed to open the window. The window object has already been "
                    "opened.");
            }

            m_IsOpen = true;
            m_Configurations = configurations;
        }

        inline void Close() override
        {
            if (!m_IsOpen)
            {
                KITSUNE_ENGINE_WARN(
                    Display,
                    "Failed to close the window. The window object is not open.");
            }

            m_IsOpen = false;
        }

        [[nodiscard]]
        inline bool IsOpen() const override
        {
            return m_IsOpen;
        }

    public:
        [[nodiscard]]
        inline Vector2<Uint32> GetSize() const override
        {
            VerifyWindowIsOpen();
            return m_Configurations.Size;
        }

        [[nodiscard]]
        inline Vector2<Int32> GetPosition() const override
        {
            VerifyWindowIsOpen();
            return m_Configurations.Position;
        }

        [[nodiscard]]
        inline String GetTitle() const override
        {
            VerifyWindowIsOpen();
            return m_Configurations.Title;
        }

        [[nodiscard]]
        inline WindowState GetState() const override
        {
            VerifyWindowIsOpen();
            return m_Configurations.State;
        }

        [[nodiscard]]
        inline bool IsVisible() const override
        {
            VerifyWindowIsOpen();
            return !bool(m_Configurations.Flags & WindowCreationFlags::Hidden);
        }

    public:
        [[nodiscard]]
        inline bool IsUserResizable() const override
        {
            VerifyWindowIsOpen();

            if ((m_Configurations.State == WindowState::Fullscreen) ||
                (m_Configurations.State == WindowState::Minimized))
            {
                return false;
            }

            return bool(m_Configurations.Flags & WindowCreationFlags::ResizeEnabled);
        }

    public:
        inline void SetSize(const Vector2<Uint32>& size) override
        {
            VerifyWindowIsOpen();
            if (m_Configurations.State != WindowState::Windowed)
            {
                KITSUNE_ENGINE_WARN(
                    Display,
                    "Cannot set the size of a window which is not in the windowed "
                    "state.");
            }

            m_Configurations.Size = size;
        }

        inline void SetPosition(const Vector2<Int32>& position) override
        {
            VerifyWindowIsOpen();
            if (m_Configurations.State != WindowState::Windowed)
            {
                KITSUNE_ENGINE_WARN(
                    Display,
                    "Cannot set the position of a window which is not in the windowed "
                    "state.");
            }

            m_Configurations.Position = position;
        }

        inline void SetTitle(StringView title) override
        {
            VerifyWindowIsOpen();
            m_Configurations.Title = title;
        }

        void SetState(WindowState state) override
        {
            VerifyWindowIsOpen();
            m_Configurations.State = state;
        }

        void SetVisibility(bool visible) override
        {
            VerifyWindowIsOpen();

            if (!visible)
                m_Configurations.Flags |= WindowCreationFlags::Hidden;
            else
                m_Configurations.Flags &= ~WindowCreationFlags::Hidden;
        }

    private:
        inline void VerifyWindowIsOpen() const
        {
            if (!m_IsOpen)
            {
                // Not necessarily a "system" exception, but users expect windows to
                // throw SystemException's.
                throw SystemException(
                    "This operation is invalid on this window. The window object is "
                    "not in the open state.");
            }
        }

    private:
        bool m_IsOpen = false;
        WindowConfigurations m_Configurations;
    };
}
