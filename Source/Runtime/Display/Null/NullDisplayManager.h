#pragma once

#include "Display/DisplayManager.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    class NullDisplayManager : public DisplayManager
    {
    public:
        inline NullDisplayManager(
            const Vector2<Uint32>& displaySize, Uint32 refreshRate,
            DisplayOrientation orientation)
            : m_DisplayInfo{
                  .Size = displaySize,
                  .Position = { 0, 0 },
                  .RefreshRate = refreshRate,
                  .Scaling = 1.0f,
                  .Orientation = orientation,
                  .MainDisplay = true
              }
        {
        }

    public:
        inline void Update(double delta) override
        {
            KITSUNE_UNUSED(delta);
        }

    public:
        [[nodiscard]]
        inline Array<DisplayId> GetDisplays() const override
        {
            return { GetMainDisplay() };
        }

        [[nodiscard]]
        inline DisplayId GetMainDisplay() const override
        {
            return DisplayId();
        }

        [[nodiscard]]
        inline Usize GetDisplayCount() const override
        {
            return 1;
        }

    public:
        [[nodiscard]]
        inline DisplayInformation GetDisplayInformation(
            DisplayId displayId) const override
        {
            if (!IsDisplayConnected(displayId))
            {
                throw InvalidArgumentException(
                    "Tried to set the orientation of an invalid display.");
            }

            return m_DisplayInfo;
        }

        [[nodiscard]]
        inline bool IsDisplayConnected(DisplayId displayId) const override
        {
            return (displayId == GetMainDisplay());
        }

    public:
        inline void SetDisplayOrientation(
            DisplayId displayId,
            DisplayOrientation orientation) override
        {
            if (!IsDisplayConnected(displayId))
            {
                throw InvalidArgumentException(
                    "Tried to set the orientation of an invalid display.");
            }

            m_DisplayInfo.Orientation = orientation;
        }

    private:
        DisplayInformation m_DisplayInfo;
    };
}
