#pragma once

#include "Application/IScreen.h"
#include "Application/IWindow.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Containers/Array.h"

namespace Kitsune
{
    class IDisplayManager
    {
    public:
        IDisplayManager();
        virtual ~IDisplayManager();

    public:
        virtual void Update() = 0;

        [[nodiscard]] virtual SharedPtr<IScreen> GetPrimaryScreen() const = 0;
        [[nodiscard]] virtual Array<SharedPtr<IScreen>> GetScreens() const = 0;

    public:
        virtual SharedPtr<IWindow> RegisterWindow(const WindowSpecifications& specs) = 0;
        [[nodiscard]] virtual SharedPtr<IWindow> GetPrimaryWindow() const = 0;

    public:
        [[nodiscard]]
        inline static IDisplayManager* GetInstance()
        {
            return s_Instance;
        }

    private:
        static IDisplayManager* s_Instance;
    };
}
