#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Utilities/NonCopyable.h"

#include "Application/CommandLineArguments.h"

namespace Kitsune
{
    struct ApplicationSpecifications
    {
        String Name;
        String Description;

        String Version;
    };

    class IApplication : public NonCopyable
    {
    public:
        IApplication(const ApplicationSpecifications& specs);
        virtual ~IApplication() { /* ... */ }

    public:
        virtual void OnUpdate() { /* ... */ }

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
