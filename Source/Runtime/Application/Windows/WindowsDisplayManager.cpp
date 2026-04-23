#include "Application/Windows/WindowsDisplayManager.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/String/TranscodePresets.h"

#include "Foundation/Diagnostics/Assert.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    WindowsDisplayManager::WindowsDisplayManager(
        WideStringView className,
        const DisplayManagerSpecifications& specs)
    {
        KITSUNE_ENGINE_INFO_FORMAT(
            DisplayManager,
            "Creating the Windows implementation of a DisplayManager, using "
            "\"{0}\" as the primary window's WNDCLASSEX window name.",
            Utf16ToUtf8<wchar_t, char>(className));

        RegisterWindowClass_(className);
        UpdateScreenList_();

        m_PrimaryWindow = Memory::New<WindowsWindow>(
            m_WindowClassName,
            specs.PrimaryWindowSpecs);

        KITSUNE_ENGINE_INFO(
            DisplayManager,
            "The Windows implementation of DisplayManager has been "
            "successfully initialized!");
    }

    WindowsDisplayManager::~WindowsDisplayManager()
    {
        Memory::Delete(m_PrimaryWindow);
        KITSUNE_VERIFY(
            ::UnregisterClassW(m_WindowClassName.Raw(), nullptr),
            "Failed to unregister the window class.");

        KITSUNE_ENGINE_INFO(
            DisplayManager,
            "The Windows display manager has been destroyed.");
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

    Screen* WindowsDisplayManager::GetPrimaryScreen() const
    {
        if (m_Screens.IsEmpty())
            return nullptr;

        return m_Screens[0].Get();
    }

    Array<Screen*> WindowsDisplayManager::GetScreens() const
    {
        Array<Screen*> screens;
        for (const ScopedPtr<WindowsScreen>& screen : m_Screens)
            screens.PushBack(screen.Get());

        return screens;
    }

    void WindowsDisplayManager::RegisterWindowClass_(
        WideStringView className)
    {
        WNDCLASSEXW windowClass;
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;

        windowClass.lpfnWndProc = WindowsDisplayManager::WindowProcedure_;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;

        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = className.Data();

        windowClass.hInstance = nullptr;
        windowClass.hIcon = nullptr;
        windowClass.hIconSm = nullptr;
        windowClass.hbrBackground = nullptr;

        HANDLE cursorImage = ::LoadImage(
            nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0,
            LR_DEFAULTSIZE | LR_SHARED);

        windowClass.hCursor = reinterpret_cast<HCURSOR>(cursorImage);

        if (!::RegisterClassExW(&windowClass))
            throw SystemException("Failed to register the window class.");

        m_WindowClassName = className;
    }

    void WindowsDisplayManager::UpdateScreenList_()
    {
        DISPLAY_DEVICEW device;
        device.cb = sizeof(DISPLAY_DEVICEW);

        const auto predicate =
            [&device](const ScopedPtr<WindowsScreen>& screen) -> bool
            {
                return (screen->GetDeviceName() == device.DeviceName);
            };

        Usize prevConnectedCount = m_Screens.Size();
        Array<ScopedPtr<WindowsScreen>> connected(prevConnectedCount);

        for (DWORD index = 0; /* ... */; ++index)
        {
            if (!::EnumDisplayDevicesW(nullptr, index, &device, 0))
                break;

            if (!(device.StateFlags & DISPLAY_DEVICE_ACTIVE))
                continue;

            auto iter = Algorithms::FindIf(m_Screens.GetBegin(), m_Screens.GetEnd(),
                                           predicate);

            if (iter == m_Screens.GetEnd())
            {
                connected.PushBack(MakeScoped<WindowsScreen>(device.DeviceName));
                OnScreenConnected_(connected.Back().Get());
            }
            else
            {
                if (!(device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
                    connected.PushBack(Move(*iter));
                else
                {
                    // TODO: Insert should be done without preserving order,
                    //       a.k.a add an InsertUnsorted() function.
                    connected.Insert(
                        connected.GetBegin(),
                        Move(*iter));
                }

                m_Screens.Remove(iter);
            }
        }

        for (ScopedPtr<WindowsScreen>& disconnected : m_Screens)
            OnScreenDisconnected_(disconnected.Get());

        Swap(m_Screens, connected);
    }

    void WindowsDisplayManager::OnScreenConnected_(Screen* screen)
    {
        // Avoid calling screen connection code during the first discovery of
        // screens in the constructor.
        if (DisplayManager::GetInstance() == nullptr)
            return;

        KITSUNE_ENGINE_INFO_FORMAT(
            DisplayManager,
            "Screen connected! {0}", *screen);
    }

    void WindowsDisplayManager::OnScreenDisconnected_(Screen* screen)
    {
        KITSUNE_ENGINE_INFO_FORMAT(
            DisplayManager,
            "Screen disconnected! {0}", *screen);
    }

    LRESULT WindowsDisplayManager::WindowProcedure_(
        HWND windowHandle, UINT message, WPARAM wparam,
        LPARAM lparam)
    {
        // Make sure that all messages sent before the window is fully initialized
        // gets routed over, as to not break anything.
        auto* window = reinterpret_cast<WindowsWindow*>(
            ::GetWindowLongPtrW(windowHandle, GWLP_USERDATA));

        auto* displayManager = dynamic_cast<WindowsDisplayManager*>(
            DisplayManager::GetInstance());

        if ((window == nullptr) || (displayManager == nullptr) ||
            (displayManager->m_PrimaryWindow == nullptr))
        {
            return HandlePreInitWindowEvents_(
                windowHandle, message,
                wparam, lparam);
        }

        // The window handle has been retrieved and stored in the display manager.
        return HandlePostInitWindowEvents_(
            window, message, wparam, lparam);
    }

    LRESULT WindowsDisplayManager::HandlePreInitWindowEvents_(
        HWND windowHandle, UINT message, WPARAM wparam,
        LPARAM lparam)
    {
        return DefWindowProcW(
            windowHandle, message, wparam,
            lparam);
    }

    LRESULT WindowsDisplayManager::HandlePostInitWindowEvents_(
        WindowsWindow* window, UINT message, WPARAM wparam,
        LPARAM lparam)
    {
        return DefWindowProcW(
            window->GetNativeHandle(), message, wparam,
            lparam);
    }
}
