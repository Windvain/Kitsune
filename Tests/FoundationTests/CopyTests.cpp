#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/Copy.h"

using namespace Kitsune;
using namespace Testing;

TEST(CopyTests, Copy)
{
    ForwardTestContainer<int, 5> container({ 2, 3, 1, 4, 6 });
    ForwardTestContainer<int, 4> destContainer({ 54, 1, 2, 6 });

    std::vector<int> expected = { 2, 3, 1, 4, 6 };
    std::vector<int> destExpected = { 2, 3, 1, 4 };

    auto endRange = container.GetBegin();
    ++endRange; ++endRange; ++endRange; ++endRange;

    auto it = Algorithms::Copy(
        container.GetBegin(), endRange, destContainer.GetBegin());

    EXPECT_EQ(it, destContainer.GetEnd());

    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(container[i], expected[i]);

    for (std::size_t i = 0; i < 4; ++i)
        EXPECT_EQ(destContainer[i], destExpected[i]);
}

TEST(CopyTests, CopyN)
{
    ForwardTestContainer<int, 5> container({ 2, 3, 1, 4, 6 });
    ForwardTestContainer<int, 4> destContainer({ 54, 1, 2, 6 });

    std::vector<int> expected = { 2, 3, 1, 4, 6 };
    std::vector<int> destExpected = { 2, 3, 1, 4 };

    auto it = Algorithms::CopyN(
        container.GetBegin(), 4, destContainer.GetBegin());

    EXPECT_EQ(it, destContainer.GetEnd());

    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(container[i], expected[i]);

    for (std::size_t i = 0; i < 4; ++i)
        EXPECT_EQ(destContainer[i], destExpected[i]);
}

TEST(CopyTests, CopyIf)
{
    ForwardTestContainer<int, 5> container({ 2, 3, 1, 4, 6 });
    ForwardTestContainer<int, 4> destContainer({ 54, 1, 2, 6 });

    std::vector<int> expected = { 2, 3, 1, 4, 6 };
    std::vector<int> destExpected = { 2, 4, 6, 6 };

    auto it = Algorithms::CopyIf(
        container.GetBegin(), container.GetEnd(), destContainer.GetBegin(),
        [](int elem) -> bool
        {
            return (elem % 2) == 0;
        });

    auto expectedIt = destContainer.GetBegin();
    ++expectedIt; ++expectedIt; ++expectedIt;

    EXPECT_EQ(it, expectedIt);

    for (std::size_t i = 0; i < 5; ++i)
        EXPECT_EQ(container[i], expected[i]);

    for (std::size_t i = 0; i < 4; ++i)
        EXPECT_EQ(destContainer[i], destExpected[i]);
}

TEST(CopyTests, CopyBackwards)
{
    BidirTestContainer<int, 8> container({ 3, 2, 84, 1, 0, 9, 11, 17 });
    std::vector<int> vec = { 3, 2, 84, 3, 2, 84, 1, 0 };

    auto endRange = container.GetBegin();
    ++endRange; ++endRange; ++endRange; ++endRange; ++endRange;

    auto it = Algorithms::CopyBackwards(container.GetBegin(), endRange, container.GetEnd());
    auto expectedIt = container.GetBegin();

    ++expectedIt; ++expectedIt; ++expectedIt;
    EXPECT_EQ(it, expectedIt);

    ASSERT_EQ(vec.size(), 8);
    for (std::size_t i = 0; i < 8; ++i)
        EXPECT_EQ(container[i], vec[i]);
}
