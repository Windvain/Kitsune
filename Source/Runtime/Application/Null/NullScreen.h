#pragma once

#include "Application/Screen.h"

namespace Kitsune
{
    class NullScreen : public Screen
    {
    public:
        inline NullScreen(const Vector2<Uint32>& size,
                          const Vector2<Int32>& position,
                          Uint32 refreshRate,
                          Uint32 dpi,
                          ScreenOrientation orientation)
            : m_Size(size),
              m_Position(position),
              m_RefreshRate(refreshRate),
              m_Dpi(dpi),
              m_Orientation(orientation)
        {
        }

        ~NullScreen() override = default;

    public:
        [[nodiscard]]
        inline String GetName() const override
        {
            return "NullScreen";
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
        inline Uint32 GetRefreshRate() const override
        {
            return m_RefreshRate;
        }

        [[nodiscard]]
        inline Uint32 GetDotsPerInch() const override
        {
            return m_Dpi;
        }

        [[nodiscard]]
        inline ScreenOrientation GetOrientation() const override
        {
            return m_Orientation;
        }

    public:
        inline void SetSize(const Vector2<Uint32>& size) override
        {
            m_Size = size;
        }

        inline void SetOrientation(ScreenOrientation orientation) override
        {
            m_Orientation = orientation;
        }

    private:
        Vector2<Uint32> m_Size;
        Vector2<Int32> m_Position;

        Uint32 m_RefreshRate;
        Uint32 m_Dpi;

        ScreenOrientation m_Orientation;
    };
}
