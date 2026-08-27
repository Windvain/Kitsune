#include <memory>
#include <gtest/gtest.h>
#include "TestContainer.h"

#include "Foundation/Algorithms/Copy.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::BidirectionalTestContainer;

    // Algorithms::Copy(Iter, Iter, OutIter)
    TEST(CopyTest, Copy)
    {
        using ContainerType = ForwardTestContainer<std::shared_ptr<int>, 5>;
        ContainerType source = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(source[index].use_count(), 1);

        ContainerType destination = {
            std::make_shared<int>(213123),
            std::make_shared<int>(4517),
            std::make_shared<int>(81),
            std::make_shared<int>(23),
            std::make_shared<int>(91),
        };

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
    TEST(CopyTest, CopyN)
    {
        using ContainerType = ForwardTestContainer<std::shared_ptr<int>, 5>;
        ContainerType source = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(source[index].use_count(), 1);

        ContainerType destination = {
            std::make_shared<int>(213123),
            std::make_shared<int>(4517),
            std::make_shared<int>(81),
            std::make_shared<int>(23),
            std::make_shared<int>(91),
        };

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
    TEST(CopyTest, CopyIf)
    {
        using ContainerType = ForwardTestContainer<std::shared_ptr<int>, 5>;
        ContainerType source = {
            std::make_shared<int>(20),
            std::make_shared<int>(32),
            std::make_shared<int>(14),
            std::make_shared<int>(698),
            std::make_shared<int>(1),
        };

        for (int index = 0; index < 5; ++index)
            ASSERT_EQ(source[index].use_count(), 1);

        ContainerType destination = {
            std::make_shared<int>(213123),
            std::make_shared<int>(4517),
            std::make_shared<int>(81),
            std::make_shared<int>(23),
            std::make_shared<int>(23231),
        };

        const auto predicate = [](const std::shared_ptr<int>& pointer) -> bool
        {
            return ((*pointer % 2) == 0);
        };

        auto iterator = Algorithms::CopyIf(
            source.GetBegin(),
            source.GetEnd(),
            destination.GetBegin(),
            predicate);

        auto expected = destination.GetEnd();
        EXPECT_EQ(iterator.Pointer(), &destination[4]);

        std::vector<int> sourceExpected = { 20, 32, 14, 698, 1 };
        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(*source[index], sourceExpected[index]);

        std::vector<int> destExpected = { 20, 32, 14, 698, 23231 };
        for (int index = 0; index < 4; ++index)
        {
            EXPECT_EQ(source[index].use_count(), 2);

            EXPECT_EQ(destination[index].use_count(), 2);
            EXPECT_EQ(*destination[index], destExpected[index]);
        }

        EXPECT_EQ(source[4].use_count(), 1);
        EXPECT_EQ(destination[4].use_count(), 1);
    }

    // Algorithms::CopyBackwards(Iter, Iter, OutIter)
    TEST(CopyTest, CopyBackwards)
    {
        using ContainerType = BidirectionalTestContainer<std::shared_ptr<int>, 5>;
        ContainerType source = {
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
