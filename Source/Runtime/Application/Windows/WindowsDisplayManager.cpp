#include "Application/Windows/WindowsDisplayManager.h"
#include "Foundation/Algorithms/Contains.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"

#if defined(KITSUNE_HAS_VULKAN_BACKEND)
    #include "RenderingCore/Vulkan/VulkanRenderingContext.h"
#endif

namespace Kitsune
{
    WindowsDisplayManager::WindowsDisplayManager(const DisplayManagerSpecifications& specs,
                                                 const WideStringView className)
        : m_WindowClassName(className)
    {
        KITSUNE_ENGINE_INFO(
            WindowsDisplayManager,
            "Creating the Windows implementation of DisplayManager.");

        WNDCLASSEXW windowClass;
        HANDLE cursorImage = ::LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0,
                                         LR_DEFAULTSIZE | LR_SHARED);

        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;

        windowClass.lpfnWndProc = WindowsDisplayManager::WindowProc_;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;

        windowClass.hInstance = nullptr;
        windowClass.hIcon = nullptr;
        windowClass.hIconSm = nullptr;
        windowClass.hbrBackground = nullptr;
        windowClass.hCursor = reinterpret_cast<HCURSOR>(cursorImage);

        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = m_WindowClassName.Raw();

        if (!::RegisterClassExW(&windowClass))
            throw SystemException("Failed to register the window class.");

        // Engine code might try to read screen data before the first frame,
        // so update just in case.
        UpdateScreenList_();

        // Create the RenderingContext first.
        switch (specs.Backend)
        {
        case RenderingBackend::Vulkan:
        {
#if defined(KITSUNE_HAS_VULKAN_BACKEND)
            m_RenderingContext = Memory::New<VulkanRenderingContext>();
#endif
            break;
        }
        }

        if (m_RenderingContext == nullptr)
        {
            throw SystemException(
                "The engine was built with no valid rendering backend, or a "
                "valid rendering backend was not picked. The rendering subsystem "
                "will not be initialized.");
        }

        // Then the window.
        const WindowSpecifications& primaryWindowSpecs = specs.PrimaryWindowSpecs;
        m_PrimaryWindow = Memory::New<WindowsWindow>(
            className,
            primaryWindowSpecs.Size,
            primaryWindowSpecs.Position,
            Utf8ToUtf16<char, wchar_t>(primaryWindowSpecs.Title),
            primaryWindowSpecs.Mode,
            primaryWindowSpecs.Flags);

        m_RenderingDevice = m_RenderingContext->CreateRenderingDevice(
            Uint32(0),
            m_PrimaryWindow);
    }

    WindowsDisplayManager::~WindowsDisplayManager()
    {
        KITSUNE_ASSERT(
            m_PrimaryWindow != nullptr,
            "The primary window has not been created.");

        if (m_RenderingContext != nullptr)
            m_RenderingContext->DestroyRenderingDevice(m_RenderingDevice);

        Memory::Delete(m_PrimaryWindow);

        KITSUNE_VERIFY(::UnregisterClassW(m_WindowClassName.Raw(), nullptr),
                       "Failed to unregister the window class.");
    }

    void WindowsDisplayManager::Update()
    {
        MSG message;
        while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }

        UpdateScreenList_();
    }

    ScreenHandle WindowsDisplayManager::GetPrimaryScreen() const
    {
        if (m_Screens.IsEmpty())
            return nullptr;

        return m_Screens[0].Get();
    }

    Array<ScreenHandle> WindowsDisplayManager::GetScreens() const
    {
        Array<ScreenHandle> screenHandles;
        for (const ScopedPtr<WindowsScreen>& screen : m_Screens)
            screenHandles.PushBack(screen.Get());

        return screenHandles;
    }

    WindowHandle WindowsDisplayManager::GetPrimaryWindow() const
    {
        return m_PrimaryWindow;
    }

    void WindowsDisplayManager::UpdateScreenList_()
    {
        DISPLAY_DEVICEW device;
        device.cb = sizeof(DISPLAY_DEVICEW);

        Usize prevConnected = m_Screens.Size();
        Array<ScopedPtr<WindowsScreen>> connectedScreens(prevConnected);

        for (DWORD index = 0; /* ... */; ++index)
        {
            if (!::EnumDisplayDevicesW(nullptr, index, &device, 0))
                break;

            if (!(device.StateFlags & DISPLAY_DEVICE_ACTIVE))
                continue;

            auto predicate = [&device](const ScopedPtr<WindowsScreen>& screen) -> bool
            {
                return (screen->GetDeviceName() == device.DeviceName);
            };

            auto iter = Algorithms::FindIf(m_Screens.GetBegin(), m_Screens.GetEnd(), predicate);
            if (iter == m_Screens.GetEnd())
            {
                connectedScreens.PushBack(MakeScoped<WindowsScreen>(device.DeviceName));
            }
            else
            {
                // TODO: Insert should be done without preserving order, a.k.a
                //       add an InsertUnsorted() function.
                if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
                    connectedScreens.Insert(connectedScreens.GetBegin(), Move(*iter));
                else
                    connectedScreens.PushBack(Move(*iter));

                m_Screens.Remove(iter);
            }
        }

        for (ScopedPtr<WindowsScreen>& disconnected : m_Screens)
        {
            KITSUNE_UNUSED(disconnected);
        }

        Swap(m_Screens, connectedScreens);
    }

    LRESULT WindowsDisplayManager::WindowProc_(HWND windowHandle, UINT message, WPARAM wparam,
                                               LPARAM lparam)
    {
        // Make sure that the window has been properly initialized before checking for events.
        // There might be unexpected consequences otherwise.
        LONG_PTR windowPointer = ::GetWindowLongPtrW(windowHandle, GWLP_USERDATA);
        auto* displayManager = dynamic_cast<WindowsDisplayManager*>(DisplayManager::GetInstance());

        if (!windowPointer || !displayManager)
            return HandlePreInitWindowEvents_(windowHandle, message, wparam, lparam);

        auto window = reinterpret_cast<WindowsWindow*>(windowPointer);
        if (displayManager->m_PrimaryWindow == nullptr)
            return HandlePreInitWindowEvents_(windowHandle, message, wparam, lparam);

        // Window has already been created (an HWND exists) and has been kept in the
        // display manager.
        return HandlePostInitWindowEvents_(window, message, wparam, lparam);
    }

    LRESULT WindowsDisplayManager::HandlePreInitWindowEvents_(HWND windowHandle, UINT message,
                                                              WPARAM wparam, LPARAM lparam)
    {
        return DefWindowProcW(windowHandle, message, wparam, lparam);
    }

    LRESULT WindowsDisplayManager::HandlePostInitWindowEvents_(WindowsWindow* window, UINT message,
                                                               WPARAM wparam, LPARAM lparam)
    {
        return DefWindowProcW(window->GetNativeHandle(), message, wparam, lparam);
    }
}
