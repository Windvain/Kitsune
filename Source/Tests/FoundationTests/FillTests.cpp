#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/Fill.h"

using namespace Kitsune;
using namespace Testing;

TEST(FillTests, FillRange)
{
    ForwardTestContainer<int, 6> container({ 32, 6, 22, 6, 12, 222 });
    std::vector<int> expected = { 655, 655, 655, 655, 12, 222 };

    auto rangeEnd = container.GetBegin();
    ++rangeEnd; ++rangeEnd; ++rangeEnd; ++rangeEnd;

    Algorithms::Fill(container.GetBegin(), rangeEnd, 655);
    for (std::size_t i = 0; i < 6; ++i)
        EXPECT_EQ(container[i], expected[i]);
}
