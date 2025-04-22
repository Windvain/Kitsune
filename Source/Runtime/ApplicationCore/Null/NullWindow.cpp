#include "ApplicationCore/Null/NullWindow.h"
#include "ApplicationCore/Application.h"

namespace Kitsune
{
    NullWindow::NullWindow(const WindowProperties& props)
        : m_Size(props.Size), m_Position(props.Position),
          m_RestoreSize(props.Size), m_RestorePosition(props.Position),
          m_Title(props.Title), m_State(props.State)
    {
    }

    void NullWindow::SetState(WindowState state)
    {
        if (state == m_State)
            return;

        if (state == WindowState::Windowed)
        {
            m_Position = m_RestorePosition;
            m_Size = m_RestoreSize;
        }
        else
        {
            Application& app = Application::GetInstance();
            if (m_State == WindowState::Windowed)
            {
                m_RestorePosition = m_Position;
                m_RestoreSize = m_Size;
            }

            m_Position = { 0, 0 };
            m_Size = ((state == WindowState::Maximized) || (state == WindowState::Fullscreen)) ?
                app.GetPrimaryMonitor()->GetVideoMode().Resolution :
                Vector2<Uint32>(0, 0);
        }

        m_State = state;
    }
}
