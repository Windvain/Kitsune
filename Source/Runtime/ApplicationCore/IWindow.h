#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/Maths/AABB.h"

#include "Foundation/String/String.h"
#include "Foundation/Memory/SharedPtr.h"

#include "ApplicationCore/VideoMode.h"

KITSUNE_PUSH_COMPILER_WARNINGS()

KITSUNE_IGNORE_MSVC_WARNING(4100)       // 'XXX': Unreferenced format parameter (in virtual functions)
KITSUNE_IGNORE_CLANG_WARNING(-Wunused-parameter)

namespace Kitsune
{
    enum class WindowState
    {
        Floating,
        Minimized,
        Maximized
    };

    enum class WindowPositionHint
    {
        UsePosition,
        DefaultPosition,
        ScreenCenter
    };

    struct WindowProperties
    {
        Vector2<Int32> Position;
        Vector2<Uint32> Size = { 640, 480 };

        String Title;
        VideoMode VideoMode;

        WindowState WindowState = WindowState::Floating;
        WindowPositionHint PositionHint = WindowPositionHint::UsePosition;
    };

    class IWindow
    {
    public:
        virtual ~IWindow() { /* ... */ }

    public:
        virtual Vector2<Uint32> GetSize() const = 0;
        virtual Vector2<Int32> GetPosition() const = 0;

        virtual void SetSize(const Vector2<Uint32>& size) { /* ... */ }
        virtual void SetPosition(const Vector2<Int32>& pos) { /* ... */ }

    public:
        virtual AABB2<Int32> GetFrameBoundingBox() const = 0;
        inline AABB2<Int32> GetBoundingBox() const
        {
            Vector2<Int32> pos = GetPosition();
            return AABB2<Int32>(pos, pos + GetSize());
        }

        inline void SetBoundingBox(const AABB2<Int32>& newBB)
        {
            SetPosition(newBB.TopLeft);
            SetSize(newBB.BottomRight - newBB.TopLeft);
        }

    public:
        virtual void SetTitle(StringView title) = 0;
        virtual String GetTitle() const = 0;

    public:
        virtual void Minimize() { /* ... */ }
        virtual void Maximize() { /* ... */ }
        virtual void Restore() { /* ... */ }

        virtual bool IsMinimized() const = 0;
        virtual bool IsMaximized() const = 0;
        virtual bool IsFloating() const = 0;

    public:
        virtual void Show() { /* ... */ }
        virtual void Hide() { /* ... */ }

        virtual bool IsShown() const = 0;
    };

    KITSUNE_API_ SharedPtr<IWindow> MakeWindow(const WindowProperties& props);
}

KITSUNE_POP_COMPILER_WARNINGS()
