#pragma once

#include "ApplicationCore/IWindow.h"

namespace Kitsune
{
    class NullWindow : public IWindow
    {
    public:
        KITSUNE_API_ NullWindow(const WindowProperties& props);

    public:
        [[nodiscard]] inline Vector2<Uint32> GetSize()    const override { return m_Size; }
        [[nodiscard]] inline Vector2<Int32> GetPosition() const override { return m_Position; }

        inline void SetSize(const Vector2<Uint32>& size)   override { m_Size = size; }
        inline void SetPosition(const Vector2<Int32>& pos) override { m_Position = pos; }

    public:
        inline void SetTitle(StringView title) override { m_Title = title; }
        [[nodiscard]] inline String GetTitle() const override { return m_Title; }

    public:
        KITSUNE_API_ void SetState(WindowState state) override;
        [[nodiscard]] inline WindowState GetState() const override { return m_State; }

        inline void Minimize() override   { return SetState(WindowState::Minimized); }
        inline void Maximize() override   { return SetState(WindowState::Maximized); }
        inline void Restore()  override   { return SetState(WindowState::Windowed); }
        inline void Fullscreen() override { SetState(WindowState::Fullscreen); }

        [[nodiscard]] inline bool IsMinimized()  const override { return GetState() == WindowState::Minimized; }
        [[nodiscard]] inline bool IsMaximized()  const override { return GetState() == WindowState::Maximized; }
        [[nodiscard]] inline bool IsWindowed()   const override { return GetState() == WindowState::Windowed; }
        [[nodiscard]] inline bool IsFullscreen() const override { return GetState() == WindowState::Fullscreen; }

    public:
        void Show() override { /* ... */ }
        void Hide() override { /* ... */ }

        bool IsShown() const override { return false; }

    private:
        Vector2<Uint32> m_Size;
        Vector2<Int32> m_Position;

        Vector2<Uint32> m_RestoreSize;
        Vector2<Int32> m_RestorePosition;

        String m_Title;
        WindowState m_State;
    };
}
