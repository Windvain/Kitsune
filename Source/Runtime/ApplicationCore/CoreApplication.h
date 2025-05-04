#pragma once

#include "Foundation/Algorithms/Find.h"
#include "Foundation/Containers/Array.h"

#include "Foundation/Memory/SharedPtr.h"
#include "Foundation/Diagnostics/Assert.h"

#include "ApplicationCore/IWindow.h"
#include "ApplicationCore/WindowException.h"

namespace Kitsune
{
    class CoreApplication
    {
    public:
        inline CoreApplication()
        {
            KITSUNE_ASSERT(s_Instance == nullptr, "CoreApplication has already been instanced.");
            s_Instance = this;
        }

        inline ~CoreApplication()
        {
            s_Instance = nullptr;
        }

    public:
        KITSUNE_API_ void Exit(int exitCode);

        [[noreturn]]
        KITSUNE_API_ void ForceExit(int exitCode);

    public:
        [[nodiscard]] inline bool IsExitRequested() const { return m_ExitRequested; }
        [[nodiscard]] inline int GetExitCode()      const { return m_ExitCode; }

    public:
        KITSUNE_API_ SharedPtr<IWindow> MakeWindow(const WindowProperties& windowProps);

    public:
        static CoreApplication& GetInstance()
        {
            KITSUNE_ASSERT(s_Instance != nullptr, "CoreApplication has not been instanced.");
            return *s_Instance;
        }

    private:
        inline void VerifyWindowProperties(const WindowProperties& windowProps)
        {
            if (windowProps.Size == Vector2<Uint32>())
                throw WindowException("Cannot create a window with a size of [0, 0].");
        }

    private:
        int m_ExitCode = 0;
        bool m_ExitRequested = false;

        KITSUNE_API_ static CoreApplication* s_Instance;
    };
}
