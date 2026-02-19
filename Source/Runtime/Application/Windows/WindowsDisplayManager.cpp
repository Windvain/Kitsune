#include "Application/Windows/WindowsDisplayManager.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/String/TranscodePresets.h"

namespace Kitsune
{
    WindowsDisplayManager::WindowsDisplayManager()
    {
        UpdateScreenList();
    }

    WindowsDisplayManager::~WindowsDisplayManager()
    {
    }

    void WindowsDisplayManager::Update()
    {
        UpdateScreenList();
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
        if (displayDisconnected || displayConnected)
            OnScreenEvent();
    }

    void WindowsDisplayManager::OnScreenEvent()
    {
        KITSUNE_ENGINE_INFO_("A display has been connected/disconnected!");
        for (Index index = 0; index < m_Screens.Size(); ++index)
        {
            ScopedPtr<WindowsScreen>& screen = m_Screens[index];
            Vector2<Uint32> screenSize = screen->GetSize();

            KITSUNE_ENGINE_INFO_FORMAT_(
                "Screen #{0}: {1} ({2}x{3}@{4}Hz, {5} DPI)",
                index, Utf16ToUtf8<wchar_t, char>(screen->GetDeviceName()),
                screenSize.X, screenSize.Y,
                screen->GetRefreshRate(), screen->GetDotsPerInch());
        }
    }

    DisplayManager* DisplayManager::Initialize(const DisplayManagerSpecifications& specs)
    {
        KITSUNE_UNUSED(specs);

        if (s_Instance != nullptr)
            return s_Instance;

        s_Instance = Memory::New<WindowsDisplayManager>();
        return s_Instance;
    }

    void DisplayManager::Shutdown()
    {
        Memory::Delete(s_Instance);
        s_Instance = nullptr;
    }
}
