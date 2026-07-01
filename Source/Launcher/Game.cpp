#include "Core/Application.h"

namespace Kitsune
{
    class Game : public Application
    {
    public:
        Game(const CommandLineArguments& arguments)
            : Application(RetrieveAppSpecs(), arguments)
        {
        }

        ~Game() override
        {
            (void)0;
        }

    public:
        void OnUpdate(double delta) override
        {
            KITSUNE_UNUSED(delta);
        }

    private:
        static ApplicationSpecifications RetrieveAppSpecs()
        {
            return { /* ... */ };
        }
    };

    Application* CreateApplication(const CommandLineArguments& arguments)
    {
        return Memory::New<Game>(arguments);
    }
}
