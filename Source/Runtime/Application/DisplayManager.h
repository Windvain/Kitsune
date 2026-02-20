#pragma once

#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    struct DisplayManagerSpecifications
    {
        bool Headless = false;
    };

    class DisplayManager : public NonCopyable
    {
    public:
        DisplayManager() = default;
        virtual ~DisplayManager()
        {
        }

    public:
        virtual void Update() = 0;

    public:
        static DisplayManager* Initialize(const DisplayManagerSpecifications& specs);
        static void Shutdown();

    public:
        [[nodiscard]]
        inline static DisplayManager* GetInstance()
        {
            return s_Instance;
        }

    private:
        static DisplayManager* s_Instance;
    };
}
