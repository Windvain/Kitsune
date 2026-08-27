#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Move.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::BidirectionalTestContainer;

    // Algorithms::Move(Iter, Iter, OutIter)
    TEST(MoveTest, Move)
    {
        using ContainerType = ForwardTestContainer<std::shared_ptr<int>, 5>;
        ContainerType container = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(container[index].use_count(), 1);

        ContainerType destination = {
            std::make_shared<int>(213123),
            std::make_shared<int>(4517),
            std::make_shared<int>(81),
            std::make_shared<int>(23),
            std::make_shared<int>(91),
        };

        auto iterator = Algorithms::Move(
            container.GetBegin(), container.GetEnd(),
            destination.GetBegin());

        EXPECT_EQ(iterator, destination.GetEnd());

        std::vector<int> expected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(container[index], nullptr);
            EXPECT_EQ(destination[index].use_count(), 1);

            EXPECT_EQ(*destination[index], expected[index]);
        }
    }

    // Algorithms::MoveN(Iter, Size, OutIter)
    TEST(MoveTest, MoveN)
    {
        using ContainerType = ForwardTestContainer<std::shared_ptr<int>, 5>;
        ContainerType container = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(container[index].use_count(), 1);

        ContainerType destination = {
            std::make_shared<int>(213123),
            std::make_shared<int>(4517),
            std::make_shared<int>(81),
            std::make_shared<int>(23),
            std::make_shared<int>(91),
        };

        auto iterator = Algorithms::MoveN(
            container.GetBegin(), 5,
            destination.GetBegin());

        EXPECT_EQ(iterator, destination.GetEnd());

        std::vector<int> expected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(container[index], nullptr);
            EXPECT_EQ(destination[index].use_count(), 1);

            EXPECT_EQ(*destination[index], expected[index]);
        }
    }

    // Algorithms::MoveBackwards(Iter, Iter, OutIter)
    TEST(MoveTest, MoveBackwards)
    {
        using ContainerType = BidirectionalTestContainer<std::shared_ptr<int>, 5>;
        ContainerType container = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        ContainerType destination = {
            std::make_shared<int>(1134),
            std::make_shared<int>(220),
            std::make_shared<int>(123232),
            std::make_shared<int>(34245),
            std::make_shared<int>(16988),
        };

        auto iterator = Algorithms::MoveBackwards(
            container.GetBegin(), container.GetEnd(),
            destination.GetEnd());

        EXPECT_EQ(iterator, destination.GetBegin());

        std::vector<int> expected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(container[index], nullptr);
            EXPECT_EQ(destination[index].use_count(), 1);

            EXPECT_EQ(*destination[index], expected[index]);
        }
    }
}
