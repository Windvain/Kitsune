#pragma once

#include "Foundation/Maths/Vector2.h"
#include "Foundation/String/String.h"
#include "Foundation/Utilities/NonCopyable.h"

#include "Application/IWindow.h"
#include "Application/CommandLineArguments.h"

KITSUNE_PUSH_COMPILER_WARNINGS()

KITSUNE_IGNORE_MSVC_WARNING(4100)       // warning 4100: 'meow' unreferenced parameter.
KITSUNE_IGNORE_CLANG_WARNING(-Wunused-parameter)

namespace Kitsune
{
    struct ApplicationSpecifications
    {
        String Name;
        String Description;

        String Version;

        Vector2<Uint32> ViewportSize = { 640, 480 };
        WindowFlags WindowFlags = WindowFlags::None;

        Vector2<Int32> WindowPosition;
        WindowPositionHint WindowPositionHint = WindowPositionHint::PrimaryScreenCenter;
    };

    class IApplication : public NonCopyable
    {
    public:
        IApplication(const ApplicationSpecifications& specs);
        virtual ~IApplication() { /* ... */ }

    public:
        virtual void OnUpdate() { /* ... */ }

        // TODO: Make sure that the viewport is passed in to this function.
        virtual void OnViewportResize(const Vector2<Uint32>& size) { /* ... */ }

    public:
        [[nodiscard]] String GetName()        const { return m_ApplicationSpecs.Name; }
        [[nodiscard]] String GetDescription() const { return m_ApplicationSpecs.Description; }
        [[nodiscard]] String GetVersion()     const { return m_ApplicationSpecs.Version; }

    private:
        ApplicationSpecifications m_ApplicationSpecs;
    };

    // Should be defined in client code.
    extern IApplication* CreateApplication(const CommandLineArguments& cmdLineArgs);
}

KITSUNE_POP_COMPILER_WARNINGS()
