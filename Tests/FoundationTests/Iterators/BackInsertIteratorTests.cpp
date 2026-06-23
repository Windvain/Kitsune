#include <gtest/gtest.h>
#include "PushBackContainer.h"

#include "Foundation/Iterators/BackInsertIterator.h"

namespace
{
    using namespace Kitsune;
    using Container = Testing::PushBackContainer<int>;

    static_assert(
        BackInsertableContainer<Container>,
        "MyContainer does not satisfy the requirements for Container.");

    // BackInsertIterator<Container>::BackInsertIterator()
    TEST(BackInsertIteratorTest, DefaultConstructor)
    {
        BackInsertIterator<Container> iterator{};
        EXPECT_EQ(iterator.GetContainer(), nullptr);
    }

    // BackInsertIterator<Container>::BackInsertIterator(Container&)
    TEST(BackInsertIteratorTest, ContainerConstructor)
    {
        Container container;
        BackInsertIterator<Container> iterator(container);

        EXPECT_EQ(iterator.GetContainer(), &container);
    }

    // BackInsertIterator<Container>::operator=(const ValueType&)
    TEST(BackInsertIteratorTest, Assign)
    {
        Container container;
        BackInsertIterator<Container> iterator(container);

        iterator = 27;
        iterator = 3;

        std::vector<int> expected = { 27, 3 };
        EXPECT_EQ(container.GetVector(), expected);
    }

    /* operator++(), operator++(int), and operator*() are no-ops. */
}
