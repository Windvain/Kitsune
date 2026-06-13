#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Move.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::BidirectionalTestContainer;

    class MoveTest : public ::testing::Test
    {
    protected:
        using Type = std::shared_ptr<int>;
        using ContainerType = ForwardTestContainer<Type, 5>;

    protected:
        template<Usize N>
        inline ForwardTestContainer<Type, N> MakeContainer()
        {
            ForwardTestContainer<Type, N> container;
            for (Usize index = 0; index < N; ++index)
                container[index] = std::make_shared<int>(index * index);

            return container;
        }
    };

    // Algorithms::Move(Iter, Iter, OutIter)
    TEST_F(MoveTest, Move)
    {
        ContainerType container = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(container[index].use_count(), 1);

        ContainerType destination = MakeContainer<5>();
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
    TEST_F(MoveTest, MoveN)
    {
        ContainerType container = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(container[index].use_count(), 1);

        ContainerType destination = MakeContainer<5>();
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
    TEST_F(MoveTest, MoveBackwards)
    {
        BidirectionalTestContainer<Type, 5> container = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        BidirectionalTestContainer<Type, 5> destination = {
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
