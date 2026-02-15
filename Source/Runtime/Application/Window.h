#pragma once

#include "Foundation/Common/Types.h"

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

namespace Kitsune
{
    // Class representing a platform window.
    class Window
    {
    public:
        virtual ~Window() { /* ... */ }

    public:
        [[nodiscard]] virtual Vector2<Uint32> GetSize() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPosition() const = 0;

        [[nodiscard]] virtual String GetTitle() const = 0;

        [[nodiscard]] virtual Vector2<Uint32> GetSizeWithDecorations() const = 0;
        [[nodiscard]] virtual Vector2<Int32> GetPositionWithDecorations() const = 0;

    public:
        virtual void SetSize(const Vector2<Uint32>& size) = 0;
        virtual void SetPosition(const Vector2<Int32>& position) = 0;

        virtual void SetTitle(const StringView title) = 0;

        [[nodiscard]] virtual bool IsResizable() const = 0;

    public:
        virtual void Maximize() = 0;
        virtual void Minimize() = 0;

        virtual void Fullscreen() = 0;
        virtual void Restore() = 0;

        virtual void Show() = 0;
        virtual void Hide() = 0;

    public:
        [[nodiscard]] virtual bool IsMaximized() const = 0;
        [[nodiscard]] virtual bool IsMinimized() const = 0;

        [[nodiscard]] virtual bool IsWindowed() const = 0;
        [[nodiscard]] virtual bool IsFullscreen() const = 0;

        [[nodiscard]] virtual bool IsShown() const = 0;
    };
}
