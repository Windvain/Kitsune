#pragma once

#include "Foundation/String/String.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    enum class GraphicsDeviceType
    {
        Unknown,
        Integrated,
        Discrete
    };

    class GraphicsDeviceUUID
    {
    public:
        GraphicsDeviceUUID() = default;

        // m_UUID is initialized from the calls to memcpy.
        // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
        inline GraphicsDeviceUUID(const Byte* source)
        {
            std::memcpy(m_UUID, source, 2 * sizeof(Uint64));
        }

        inline GraphicsDeviceUUID(Uint64 high, Uint64 low)
        {
            std::memcpy(m_UUID, &high, sizeof(Uint64));
            std::memcpy(m_UUID + sizeof(Uint64), &low, sizeof(Uint64));
        }
        // NOLINTEND(cppcoreguidelines-pro-type-member-init)

        GraphicsDeviceUUID(const GraphicsDeviceUUID&) = default;
        GraphicsDeviceUUID& operator=(const GraphicsDeviceUUID&) = default;

    public:
        [[nodiscard]] inline const Byte* Raw() const { return m_UUID; }

        [[nodiscard]]
        inline Uint64 High() const
        {
            return *reinterpret_cast<const Uint64*>(m_UUID);
        }

        [[nodiscard]]
        inline Uint64 Low() const
        {
            return *reinterpret_cast<const Uint64*>(m_UUID + sizeof(Uint64));
        }

    public:
        inline bool operator==(const GraphicsDeviceUUID& uuid) const
        {
            return (std::memcmp(m_UUID, uuid.m_UUID, 2 * sizeof(Uint64)) == 0);
        }

    private:
        Byte m_UUID[2 * sizeof(Uint64)] = { /* ... */ };
    };

    class GraphicsDeviceDescription
    {
    public:
        inline GraphicsDeviceDescription(StringView name, StringView vendor,
                                         GraphicsDeviceType type,
                                         const GraphicsDeviceUUID& uuid)
            : m_Name(name), m_Vendor(vendor), m_Type(type), m_UUID(uuid)
        {
        }

    public:
        [[nodiscard]] inline String Name() const { return m_Name; }
        [[nodiscard]] inline String Vendor() const { return m_Vendor; }

        [[nodiscard]]
        inline GraphicsDeviceType Type() const
        {
            return m_Type;
        }

        [[nodiscard]]
        inline GraphicsDeviceUUID UUID() const
        {
            return m_UUID;
        }

    private:
        String m_Name;
        String m_Vendor;

        GraphicsDeviceType m_Type;
        GraphicsDeviceUUID m_UUID;
    };

    class GraphicsDevice : public NonCopyable
    {
    public:
        virtual ~GraphicsDevice() = default;

    public:
        [[nodiscard]]
        virtual GraphicsDeviceDescription GetDescription() const = 0;
    };
}
