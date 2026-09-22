#pragma once

#include "Display/DisplayManager.h"
#include "Core/CommandLineArguments.h"

namespace Kitsune
{
    struct ApplicationSpecifications
    {
        String Name;
        String DisplayServer;

        WindowConfigurations MainWindow;
    };

    class Application : public NonCopyable
    {
    public:
        KITSUNE_API Application(ApplicationSpecifications specifications,
                                CommandLineArguments arguments);

        KITSUNE_API virtual ~Application();

    public:
        KITSUNE_API void Update(float delta);

    public:
        [[nodiscard]]
        inline ScopedPtr<Window>& GetWindow()
        {
            return m_Window;
        }

        [[nodiscard]]
        inline const ScopedPtr<Window>& GetWindow() const
        {
            return m_Window;
        }

    private:
        DisplayManager* m_DisplayManager;
        ScopedPtr<Window> m_Window;

        ApplicationSpecifications m_Specifications;
        CommandLineArguments m_Arguments;
    };

    extern Application* CreateApplication(const CommandLineArguments& arguments);
}
