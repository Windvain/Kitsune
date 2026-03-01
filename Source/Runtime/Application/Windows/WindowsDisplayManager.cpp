#include "Application/Windows/WindowsDisplayManager.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Diagnostics/Assert.h"

#include "Foundation/String/TranscodePresets.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    WindowsDisplayManager::WindowsDisplayManager()
    {
        KITSUNE_ENGINE_INFO_("Constructing the Windows display manager with "
                             "specifications:");

        KITSUNE_ENGINE_INFO_FORMAT_(
            "\t-> Window Class Name: \"{0}\"",
            Utf16ToUtf8<wchar_t, char>(s_WindowClassName));

        KITSUNE_ENGINE_INFO_("");

        WNDCLASSEXW windowClass;
        windowClass.cbSize = sizeof(windowClass);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;

        windowClass.lpfnWndProc = DefWindowProcW;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;

        windowClass.hInstance = nullptr;
        windowClass.hIcon = nullptr;
        windowClass.hIconSm = nullptr;
        windowClass.hbrBackground = nullptr;
        windowClass.hCursor =
            reinterpret_cast<HCURSOR>(
                ::LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED));

        windowClass.lpszMenuName = nullptr;
        windowClass.lpszClassName = s_WindowClassName;

        if (!::RegisterClassExW(&windowClass))
            throw SystemException("Failed to register the window class.");

        // Engine code might try to read screen data before the first frame,
        // so update just in case.
        UpdateScreenList();
    }

    WindowsDisplayManager::~WindowsDisplayManager()
    {
        KITSUNE_ENGINE_INFO_("Destroying the Windows display manager.");
        KITSUNE_VERIFY(::UnregisterClassW(s_WindowClassName, nullptr),
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

        UpdateScreenList();
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

    WindowHandle WindowsDisplayManager::MakeWindow(const WindowSpecifications& specs)
    {
        KITSUNE_ENGINE_INFO_("Constructing a Windows window with specifications:");
        KITSUNE_ENGINE_INFO_FORMAT_(
            "\t-> Class Name: \"{0}\"",
            Utf16ToUtf8<wchar_t, char>(s_WindowClassName));

        KITSUNE_ENGINE_INFO_FORMAT_("\t-> Size: ({0}, {1})", specs.Size.X, specs.Size.Y);
        KITSUNE_ENGINE_INFO_FORMAT_(
            "\t-> Position: ({0}, {1})",
            specs.Position.X, specs.Position.Y);

        KITSUNE_ENGINE_INFO_FORMAT_("\t-> Title: {0}", specs.Title);
        KITSUNE_ENGINE_INFO_("");

        auto window = MakeScoped<WindowsWindow>(
            s_WindowClassName,
            specs.Size,
            specs.Position,
            Utf8ToUtf16<char, wchar_t>(specs.Title),
            specs.Mode,
            specs.Flags);

        m_Windows.PushBack(Move(window));
        return m_Windows.Back().Get();
    }

    void WindowsDisplayManager::DestroyWindow(WindowHandle window)
    {
        const auto predicate = [&](const ScopedPtr<WindowsWindow>& storedWindow)
        {
            return (window == storedWindow.Get());
        };

        auto iter = Algorithms::FindIf(m_Windows.GetBegin(), m_Windows.GetEnd(), predicate);
        if (iter == m_Windows.GetEnd())
        {
            throw SystemException("Failed to destroy a window, because the window handle "
                                  "specified was not created by MakeWindow().");
        }

        KITSUNE_ENGINE_INFO_FORMAT_("Destroying a Windows window with the handle value {0}.",
                                    window);

        m_Windows.Remove(iter);
    }

    WindowHandle WindowsDisplayManager::GetPrimaryWindow() const
    {
        if (m_Windows.IsEmpty())
            return nullptr;

        return m_Windows[0].Get();
    }

    void WindowsDisplayManager::UpdateScreenList()
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

            auto predicate = [&](const ScopedPtr<WindowsScreen>& screen) -> bool
            {
                return (screen->GetDeviceName() == device.DeviceName);
            };

            auto iter = Algorithms::FindIf(m_Screens.GetBegin(), m_Screens.GetEnd(),
                                           predicate);

            if (iter == m_Screens.GetEnd())
                connectedScreens.PushBack(MakeScoped<WindowsScreen>(device.DeviceName));
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

        bool displayDisconnected = !m_Screens.IsEmpty();
        bool displayConnected = (prevConnected != connectedScreens.Size());

        Swap(m_Screens, connectedScreens);

        // We don't really care whether the display was disconnected or connected,
        // but it's good to know.
        if (displayConnected || displayDisconnected)
            OnScreenEvent();
    }

    void WindowsDisplayManager::OnScreenEvent()
    {
        KITSUNE_ENGINE_INFO_("A display has been connected/disconnected!");
        KITSUNE_ENGINE_INFO_("Current screen list:");

        for (Index index = 0; index < m_Screens.Size(); ++index)
        {
            ScopedPtr<WindowsScreen>& screen = m_Screens[index];
            Vector2<Uint32> screenSize = screen->GetSize();

            KITSUNE_ENGINE_INFO_FORMAT_(
                "\tScreen #{0}: {1} ({2}x{3}@{4}Hz, {5} DPI)",
                index, Utf16ToUtf8<wchar_t, char>(screen->GetDeviceName()),
                screenSize.X, screenSize.Y,
                screen->GetRefreshRate(), screen->GetDotsPerInch());
        }

        KITSUNE_ENGINE_INFO_("");
    }
}
