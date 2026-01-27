#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/Reverse.h"

using namespace Kitsune;
using namespace Testing;

TEST(ReverseTests, BidirIterator)
{
    BidirTestContainer<int, 6> container = { 3, 4, 2, 65, 1, 22 };
    std::vector<int> expected = { 22, 1, 65, 2, 4, 3 };

    Algorithms::Reverse(container.GetBegin(), container.GetEnd());

    for (std::size_t i = 0; i < 6; ++i)
        EXPECT_EQ(container[i], expected[i]);
}

TEST(ReverseTests, RandomAccessIterator)
{
    RandomAccessTestContainer<int, 6> container = { 3, 4, 2, 65, 1, 22 };
    std::vector<int> expected = { 22, 1, 65, 2, 4, 3 };

    Algorithms::Reverse(container.GetBegin(), container.GetEnd());

    for (std::size_t i = 0; i < 6; ++i)
        EXPECT_EQ(container[i], expected[i]);
}
