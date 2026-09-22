#pragma once

#include "Display/Display.h"

namespace Kitsune
{
    class NullDisplay : public Display
    {
    public:
        inline NullDisplay(const Vector2<Uint32>& size, Uint32 refreshRate,
                           float scaling)
            : m_Size(size), m_Position(0, 0), m_RefreshRate(refreshRate),
              m_Scaling(scaling), m_Orientation(DisplayOrientation::Default)
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
        inline Uint32 GetRefreshRate() const override
        {
            return m_RefreshRate;
        }

        [[nodiscard]]
        inline float GetScaling() const override
        {
            return m_Scaling;
        }

    public:
        [[nodiscard]]
        inline DisplayOrientation GetOrientation() const override
        {
            return m_Orientation;
        }

        inline void SetOrientation(DisplayOrientation orientation) override
        {
            m_Orientation = orientation;
        }

    private:
        Vector2<Uint32> m_Size;
        Vector2<Int32> m_Position;

        Uint32 m_RefreshRate;
        float m_Scaling;
        DisplayOrientation m_Orientation;
    };
}
