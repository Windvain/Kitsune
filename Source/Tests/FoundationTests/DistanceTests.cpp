#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/Distance.h"

using namespace Kitsune;
using namespace Testing;

TEST(DistanceTests, NotRandomAccess)
{
    ForwardTestContainer<int, 6> container = { 2, 4, 1, 65, 7, 1 };
    EXPECT_EQ(Algorithms::Distance(container.GetBegin(), container.GetEnd()), 6);
}

TEST(DistanceTests, RandomAccess)
{
    RandomAccessTestContainer<int, 4> container = { 4, 93, 61, 3 };
    EXPECT_EQ(Algorithms::Distance(container.GetBegin(), container.GetEnd()), 4);
}
