#pragma once

namespace Kitsune
{
    class Application;

    class EngineLoop
    {
    public:
        EngineLoop(int argc, char** argv);
        ~EngineLoop();

    public:
        void Tick();

    public:
        bool IsExitRequested() const;
        int GetExitCode() const;

    private:
        Application* m_Application;
    };
}
