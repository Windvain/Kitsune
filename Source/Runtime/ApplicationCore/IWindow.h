#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"

#include "Foundation/Utilities/EnumFlags.h"
#include "Foundation/Utilities/NonCopyable.h"

KITSUNE_PUSH_COMPILER_WARNINGS()

KITSUNE_IGNORE_MSVC_WARNING(4100)       // 'XXX': Unreferenced format parameter (in virtual functions)
KITSUNE_IGNORE_CLANG_WARNING(-Wunused-parameter)

namespace Kitsune
{
    enum class WindowState
    {
        Windowed,
        Maximized,
        Minimized,
        Fullscreen
    };

    enum class WindowFlag
    {
        None = 0,
        Resizable = 1 << 0
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(WindowFlag);

    struct WindowProperties
    {
        Vector2<Int32> Position;
        Vector2<Uint32> Size;

        String Title;
        WindowState State = WindowState::Windowed;
        WindowFlag Flags = WindowFlag::Resizable;
    };

    class IWindow : public NonCopyable
    {
    public:
        virtual ~IWindow() { /* ... */ }

    public:
        virtual Vector2<Uint32> GetSize() const = 0;
        virtual Vector2<Int32> GetPosition() const = 0;

        virtual void SetSize(const Vector2<Uint32>& size) = 0;
        virtual void SetPosition(const Vector2<Int32>& pos) = 0;

    public:
        virtual void SetTitle(const StringView title) = 0;
        virtual String GetTitle() const = 0;

    public:
        virtual void SetState(WindowState state) = 0;
        virtual WindowState GetState() const = 0;

        virtual void Fullscreen() = 0;
        virtual bool IsFullscreen() const = 0;

        virtual void Minimize() = 0;
        virtual void Maximize() = 0;
        virtual void Restore() = 0;

        virtual bool IsMinimized() const = 0;
        virtual bool IsMaximized() const = 0;
        virtual bool IsWindowed() const = 0;

    public:
        virtual void Show() = 0;
        virtual void Hide() = 0;

        virtual bool IsShown() const = 0;
    };
}

KITSUNE_POP_COMPILER_WARNINGS()
