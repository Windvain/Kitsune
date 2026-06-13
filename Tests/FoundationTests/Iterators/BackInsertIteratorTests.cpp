#include <gtest/gtest.h>
#include "Foundation/Iterators/BackInsertIterator.h"

namespace
{
    using namespace Kitsune;

    class MyContainer
    {
    public:
        using ValueType = int;

        using Iterator = int*;
        using ConstIterator = const int*;

    public:
        MyContainer() = default;
        MyContainer(const MyContainer&) = default;

        MyContainer& operator=(const MyContainer&) = default;

    public:
        [[nodiscard]]
        inline Iterator GetBegin()
        {
            return m_Vector.data();
        }

        [[nodiscard]]
        inline ConstIterator GetBegin() const
        {
            return m_Vector.data();
        }

        [[nodiscard]]
        inline Iterator GetEnd()
        {
            return m_Vector.data() + m_Vector.size();
        }

        [[nodiscard]]
        inline ConstIterator GetEnd() const
        {
            return m_Vector.data() + m_Vector.size();
        }

    public:
        inline void PushBack(int element)
        {
            m_Vector.push_back(element);
        }

    public:
        [[nodiscard]]
        inline Usize Size() const
        {
            return m_Vector.size();
        }

        [[nodiscard]]
        inline bool IsEmpty() const
        {
            return m_Vector.empty();
        }

        [[nodiscard]]
        inline std::vector<int>& GetVector()
        {
            return m_Vector;
        }

        [[nodiscard]]
        inline const std::vector<int>& GetVector() const
        {
            return m_Vector;
        }

        inline void Swap(MyContainer& container)
        {
            m_Vector.swap(container.m_Vector);
        }

    public:
        inline bool operator==(const MyContainer& container) const
        {
            return m_Vector == container.m_Vector;
        }

    private:
        std::vector<int> m_Vector;
    };

    static_assert(
        BackInsertableContainer<MyContainer>,
        "MyContainer does not satisfy the requirements for Container.");

    // BackInsertIterator<Container>::BackInsertIterator()
    TEST(BackInsertIteratorTest, DefaultConstructor)
    {
        BackInsertIterator<MyContainer> iterator{};
        EXPECT_EQ(iterator.GetContainer(), nullptr);
    }

    // BackInsertIterator<Container>::BackInsertIterator(Container&)
    TEST(BackInsertIteratorTest, ContainerConstructor)
    {
        MyContainer container;
        BackInsertIterator<MyContainer> iterator(container);

        EXPECT_EQ(iterator.GetContainer(), &container);
    }

    // BackInsertIterator<Container>::operator=(const ValueType&)
    TEST(BackInsertIteratorTest, Assign)
    {
        MyContainer container;
        BackInsertIterator<MyContainer> iterator(container);

        iterator = 27;
        iterator = 3;

        std::vector<int> expected = { 27, 3 };
        EXPECT_EQ(container.GetVector(), expected);
    }

    /* operator++(), operator++(int), and operator*() are no-ops. */
}
