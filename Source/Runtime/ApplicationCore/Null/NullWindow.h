#pragma once

#include "ApplicationCore/IWindow.h"

namespace Kitsune
{
    class NullWindow : public IWindow
    {
    public:
        inline NullWindow(const WindowProperties& props)
            : m_Size(props.Size), m_Position(props.Position), m_Title(props.Title)
        {
        }

    public:
        inline Vector2<Uint32> GetSize() const override { return m_Size; }
        inline Vector2<Int32> GetPosition() const override { return m_Position; }

        inline void SetSize(const Vector2<Uint32>& size) override { m_Size = size; }
        inline void SetPosition(const Vector2<Int32>& pos) override { m_Position = pos; }

    public:
        inline AABB2<Int32> GetFrameBoundingBox() const override { return GetBoundingBox(); }

        inline void SetTitle(StringView title) override { m_Title = title; }
        inline String GetTitle() const override { return m_Title; }

    public:
        inline bool IsMaximized() const override { return (m_State == WindowState::Maximized); }
        inline bool IsMinimized() const override { return (m_State == WindowState::Minimized); }
        inline bool IsFloating() const override { return (m_State == WindowState::Floating); }

        inline void Maximize() override { m_State = WindowState::Maximized; }
        inline void Minimize() override { m_State = WindowState::Minimized; }
        inline void Restore() override { m_State = WindowState::Floating; }

    public:
        inline void Show() override { m_Visible = true; }
        inline void Hide() override { m_Visible = false; }

        inline bool IsShown() const override { return m_Visible; }

    private:
        Vector2<Uint32> m_Size;
        Vector2<Int32> m_Position;

        String m_Title;
        WindowState m_State;

        bool m_Visible;
    };

    inline SharedPtr<NullWindow> MakeNullWindow(const WindowProperties& props)
    {
        return MakeShared<NullWindow>(props);
    }
}
