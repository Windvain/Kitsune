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
        Application(const ApplicationSpecifications& specs,
                    const CommandLineArguments& arguments);

        virtual ~Application();

    public:
        virtual void OnUpdate(double delta)
        {
            KITSUNE_UNUSED(delta);
        }
    };

    extern Application* CreateApplication(const CommandLineArguments& arguments);
}
