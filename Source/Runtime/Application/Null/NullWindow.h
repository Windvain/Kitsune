#pragma once

#include "Application/Window.h"
#include "Foundation/Logging/GlobalLog.h"

namespace Kitsune
{
    class NullWindow : public Window
    {
    public:
        inline NullWindow(const Vector2<Uint32>& size,
                          const Vector2<Int32>& position,
                          const StringView title,
                          WindowMode mode,
                          WindowFlags flags)
            : m_Size(size), m_Position(position), m_Title(title),
              m_Visible(true), m_Mode(mode), m_Flags(flags)
        {
        }

    public:
        [[nodiscard]]
        inline Vector2<Uint32> GetSize() const override
        {
            return m_Size;
        }

        [[nodiscard]]
        inline Vector2<Int32> GetPosition() const override
        {
            return m_Position;
        }

        [[nodiscard]]
        inline Vector2<Uint32> GetSizeWithDecorations() const override
        {
            return m_Size;
        }

        [[nodiscard]]
        inline Vector2<Int32> GetPositionWithDecorations() const override
        {
            return m_Position;
        }

    public:
        [[nodiscard]]
        inline String GetTitle() const override
        {
            return m_Title;
        }

        [[nodiscard]]
        inline WindowMode GetMode() const override
        {
            return m_Mode;
        }

        [[nodiscard]]
        inline WindowFlags GetFlags() const override
        {
            return m_Flags;
        }

    public:
        [[nodiscard]]
        inline bool IsVisible() const override
        {
            return m_Visible;
        }

    public:
        inline void SetSize(const Vector2<Uint32>& size) override
        {
            if (m_Mode != WindowMode::Windowed)
            {
                KITSUNE_ENGINE_ERROR_FORMAT_(
                    "Tried to resize a non-restored window {0}.",
                    this);

                return;
            }

            m_Size = size;
        }

        void SetPosition(const Vector2<Int32>& position) override
        {
            if (m_Mode != WindowMode::Windowed)
            {
                KITSUNE_ENGINE_ERROR_FORMAT_(
                    "Tried to set the position of a non-restored window {0}.",
                    this);

                return;
            }

            m_Position = position;
        }

        inline void SetTitle(const StringView title) override
        {
            m_Title = title;
        }

        inline void SetMode(WindowMode mode) override
        {
            if (!IsVisible())
            {
                KITSUNE_ENGINE_ERROR_FORMAT_(
                    "Tried to set the mode of an invisible window {0}.",
                    this);

                return;
            }

            m_Mode = mode;
        }

    public:
        inline void SetVisibility(bool visible) override
        {
            m_Visible = visible;
        }

    private:
        Vector2<Uint32> m_Size;
        Vector2<Int32> m_Position;

        String m_Title;
        bool m_Visible;

        WindowMode m_Mode;
        WindowFlags m_Flags;
    };
}
