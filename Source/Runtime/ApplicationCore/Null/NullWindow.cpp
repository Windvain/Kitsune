#include "ApplicationCore/Null/NullWindow.h"

namespace Kitsune
{
    NullWindow::NullWindow(const WindowProperties& props)
    {
        m_Size = props.Size;
        m_RestoreSize = props.Size;

        m_Title = props.Title;
        m_State = props.WindowState;

        m_VideoMode = props.VideoMode;

        m_Position = GetPosFromHint(props);
        m_RestorePosition = m_Position;
    }

    void NullWindow::SetState(WindowState state)
    {
        if (state == m_State)
            return;

        if (state == WindowState::Floating)
        {
            m_Position = m_RestorePosition;
            m_Size = m_RestoreSize;
        }
        else
        {
            if (m_State == WindowState::Floating)
            {
                m_RestorePosition = m_Position;
                m_RestoreSize = m_Size;
            }

            m_Position = { 0, 0 };
            m_Size = (state == WindowState::Maximized) ?
                m_VideoMode.Resolution : Vector2<Uint32>(0, 0);
        }

        m_State = state;
    }

    Vector2<Int32> NullWindow::GetPosFromHint(const WindowProperties& props)
    {
        switch (props.PositionHint)
        {
        case WindowPositionHint::UsePosition:     return props.Position;
        case WindowPositionHint::DefaultPosition: return { 0, 0 };
        case WindowPositionHint::ScreenCenter:    return (props.VideoMode.Resolution - props.Size) / 2;
        }

        KITSUNE_UNREACHABLE();
    }
}
