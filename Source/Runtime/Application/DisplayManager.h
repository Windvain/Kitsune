#pragma once

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

#include "Application/Screen.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    struct DisplayManagerSpecifications
    {
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

        [[nodiscard]]
        virtual Array<SharedPtr<Screen>> GetScreens() const = 0;

        [[nodiscard]]
        virtual SharedPtr<Screen> GetPrimaryScreen() const = 0;

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
