#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Concepts/Container.h"

namespace Kitsune
{
    template<typename T>
    concept BackInsertableContainer =
        Container<T> &&
        requires (T container)
        {
            container.PushBack(typename T::ValueType());
        };

    // An output iterator used for pushing elements into a container. The assignment
    // operator appends to the back of the kept container using the `PushBack()`
    // member functions. The increment and dereference operators are no-ops.
    template<BackInsertableContainer Container>
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
        inline BackInsertIterator& operator++()
        {
            return *this;
        }

        inline BackInsertIterator operator++(int)
        {
            return *this;
        }

        inline BackInsertIterator& operator*()
        {
            return *this;
        }

    public:
        inline Container* GetContainer() const { return m_Container; }

    private:
        Container* m_Container = nullptr;
    };
}
