#pragma once

#include "Core/CommandLineArguments.h"

namespace Kitsune
{
    struct ApplicationSpecifications
    {
        String Name;
        bool Headless = false;
    };

    class Application : public NonCopyable
    {
    public:
        KITSUNE_API Application(const ApplicationSpecifications& specs,
                                const CommandLineArguments& arguments);

        KITSUNE_API virtual ~Application();

    public:
        virtual void OnUpdate(double delta)
        {
            KITSUNE_UNUSED(delta);
        }
    };

    extern Application* CreateApplication(const CommandLineArguments& arguments);
}
