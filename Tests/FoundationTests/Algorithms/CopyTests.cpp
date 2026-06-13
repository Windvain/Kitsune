#include <memory>
#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Copy.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::BidirectionalTestContainer;

    class CopyTest : public ::testing::Test
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

    // Algorithms::Copy(Iter, Iter, OutIter)
    TEST_F(CopyTest, Copy)
    {
        ContainerType source = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(source[index].use_count(), 1);

        ContainerType destination = MakeContainer<5>();
        auto iterator = Algorithms::Copy(
            source.GetBegin(),
            source.GetEnd(),
            destination.GetBegin());

        EXPECT_EQ(iterator, destination.GetEnd());

        std::vector<int> expected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(source[index].use_count(), 2);
            EXPECT_EQ(destination[index].use_count(), 2);

            EXPECT_EQ(*source[index], expected[index]);
            EXPECT_EQ(*destination[index], expected[index]);
        }
    }

    // Algorithms::CopyN(Iter, Size, OutIter)
    TEST_F(CopyTest, CopyN)
    {
        ContainerType source = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(source[index].use_count(), 1);

        ContainerType destination = MakeContainer<5>();
        auto iterator = Algorithms::CopyN(
            source.GetBegin(),
            5,
            destination.GetBegin());

        EXPECT_EQ(iterator, destination.GetEnd());

        std::vector<int> expected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(source[index].use_count(), 2);
            EXPECT_EQ(destination[index].use_count(), 2);

            EXPECT_EQ(*source[index], expected[index]);
            EXPECT_EQ(*destination[index], expected[index]);
        }
    }

    // Algorithms::CopyIf(Iter, Iter, OutIter, Pred)
    TEST_F(CopyTest, CopyIf)
    {
        ContainerType source = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(source[index].use_count(), 1);

        ForwardTestContainer<Type, 4> destination = MakeContainer<4>();
        const auto predicate = [](const Type& pointer) -> bool
        {
            return ((*pointer % 2) == 0);
        };

        auto iterator = Algorithms::CopyIf(
            source.GetBegin(),
            source.GetEnd(),
            destination.GetBegin(),
            predicate);

        EXPECT_EQ(iterator, destination.GetEnd());

        std::vector<int> sourceExpected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(source[index].use_count(), 2);
            EXPECT_EQ(*source[index], sourceExpected[index]);
        }

        std::vector<int> destExpected = { 20, 32, 14, 698 };
        for (int index = 0; index < 4; ++index)
        {
            EXPECT_EQ(destination[index].use_count(), 2);
            EXPECT_EQ(*destination[index], destExpected[index]);
        }
    }

    // Algorithms::CopyBackwards(Iter, Iter, OutIter)
    TEST_F(CopyTest, CopyBackwards)
    {
        BidirectionalTestContainer<Type, 5> source = {
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

        auto iterator = Algorithms::CopyBackwards(
            source.GetBegin(), source.GetEnd(), destination.GetEnd());

        EXPECT_EQ(iterator, destination.GetBegin());

        std::vector<int> expected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(source[index].use_count(), 2);
            EXPECT_EQ(destination[index].use_count(), 2);

            EXPECT_EQ(*source[index], expected[index]);
            EXPECT_EQ(*destination[index], expected[index]);
        }
    }
}
