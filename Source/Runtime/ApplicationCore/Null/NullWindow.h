#pragma once

#include "ApplicationCore/IWindow.h"

namespace Kitsune
{
    class NullWindow : public IWindow
    {
    public:
        KITSUNE_API_ NullWindow(const WindowProperties& props);

    public:
        Vector2<Uint32> GetSize()    const override { return m_Size; }
        Vector2<Int32> GetPosition() const override { return m_Position; }

        void SetSize(const Vector2<Uint32>& size)   override { m_Size = size; }
        void SetPosition(const Vector2<Int32>& pos) override { m_Position = pos; }

    public:
        AABB2<Int32> GetFrameBoundingBox() const override
        {
            return GetBoundingBox();
        }

    public:
        void SetTitle(StringView title) override { m_Title = title; }
        String GetTitle()         const override { return m_Title; }

    public:
        KITSUNE_API_ void SetState(WindowState state) override;
        WindowState GetState() const override { return m_State; }

        void Restore() override
        {
            SetState(WindowState::Floating);
        }

    public:
        void Show() override { /* ... */ }
        void Hide() override { /* ... */ }

        bool IsShown() const override { return false; }

    private:
        KITSUNE_API_ static Vector2<Int32> GetPosFromHint(
            const WindowProperties& props);

    private:
        Vector2<Uint32> m_Size;
        Vector2<Int32> m_Position;

        Vector2<Uint32> m_RestoreSize;
        Vector2<Int32> m_RestorePosition;

        String m_Title;
        WindowState m_State;

        VideoMode m_VideoMode;
    };

    inline SharedPtr<NullWindow> MakeNullWindow(const WindowProperties& props)
    {
        return MakeShared<NullWindow>(props);
    }
}
