#pragma once

#include "Foundation/Common/Types.h"

namespace Kitsune
{
    namespace Details
    {
        template<typename T>
        concept BackInsertable = requires (T container)
        {
            container.PushBack(typename T::ValueType());
        };
    }

    template<Details::BackInsertable Container>
    class BackInsertIterator
    {
    public:
        using ValueType = typename Container::ValueType;
        using DifferenceType = Ptrdiff;

    public:
        BackInsertIterator() = default;
        inline explicit BackInsertIterator(Container& container)
            : m_Container(&container)
        {
        }

    public:
        inline BackInsertIterator& operator=(const ValueType& value)
        {
            m_Container->PushBack(value);
            return *this;
        }

    public:
        inline BackInsertIterator& operator++()   { return *this; }
        inline BackInsertIterator operator++(int) { return *this; }

        inline BackInsertIterator& operator*() { return *this; }

    public:
        inline Container* GetContainer() const { return m_Container; }

    private:
        Container* m_Container = nullptr;
    };
}
