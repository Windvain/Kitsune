#pragma once

#include <utility>

#include "Foundation/Memory/Allocator.h"
#include "Foundation/Memory/GlobalAllocator.h"

namespace Kitsune::Testing
{
    class StatefulAllocator : public GlobalAllocator
    {
    public:
        StatefulAllocator() = default;
        inline explicit StatefulAllocator(int id)
            : m_Id(id)
        {
        }

        StatefulAllocator(const StatefulAllocator&) = default;
        StatefulAllocator(StatefulAllocator&& allocator)
            : m_Id(std::exchange(allocator.m_Id, 0))
        {
        }

        ~StatefulAllocator() = default;

    public:
        StatefulAllocator& operator=(const StatefulAllocator&) = default;
        StatefulAllocator& operator=(StatefulAllocator&& allocator)
        {
            m_Id = std::exchange(allocator.m_Id, 0);
            return *this;
        }

    public:
        [[nodiscard]]
        inline int GetId() const
        {
            return m_Id;
        }

    private:
        int m_Id = 0;
    };

    inline bool operator==(const StatefulAllocator& lhs, const StatefulAllocator& rhs)
    {
        return (lhs.GetId() == rhs.GetId());
    }

    static_assert(
        Allocator<StatefulAllocator>,
        "StatefulAllocator does not meet the requirements of Allocator.");
}
