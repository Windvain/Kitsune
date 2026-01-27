#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/Count.h"

using namespace Kitsune;
using namespace Testing;

TEST(CountTests, Count)
{
    ForwardTestContainer<int, 8> container = { 3, 4, 11, 6, 23, 11, 12, 11 };
    auto count = Algorithms::Count(container.GetBegin(), container.GetEnd(), 11);

    EXPECT_EQ(count, 3);
}

TEST(CountTests, CountIf)
{
    ForwardTestContainer<int, 8> container = { 3, 4, 11, 6, 23, 11, 12, 11 };
    auto count = Algorithms::CountIf(container.GetBegin(), container.GetEnd(),
        [](int elem) -> bool
        {
            return (elem % 2) != 0;
        });

    EXPECT_EQ(count, 5);
}
