#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Algorithms/ForEach.h"

using namespace Kitsune;
using namespace Testing;

TEST(ForEachTests, ForEach)
{
    ForwardTestContainer<int, 7> container({ 1, 65, 3, 87, 878, 1, 33 });
    std::vector<int> vec = { 1, 65, 3, 87, 878, 1, 33 };

    int iterations = 0;
    Algorithms::ForEach(container.GetBegin(), container.GetEnd(),
                        [&](int element)
                        {
                            EXPECT_EQ(element, vec[iterations]);
                            ++iterations;
                        });

    EXPECT_EQ(iterations, 7);
}

TEST(ForEachTests, ForEachN)
{
    ForwardTestContainer<int, 7> container({ 1, 65, 3, 87, 878, 1, 33 });
    std::vector<int> vec = { 1, 65, 3, 87, 878, 1, 33 };

    int iterations = 0;
    Algorithms::ForEachN(container.GetBegin(), 5,
                         [&](int element)
                         {
                             EXPECT_EQ(element, vec[iterations]);
                             ++iterations;
                         });

    EXPECT_EQ(iterations, 5);
}

TEST(ForEachTests, ForEachIf)
{
    ForwardTestContainer<int, 7> container({ 1, 65, 3, 87, 878, 1, 33 });
    std::vector<int> vec = { 878 };

    auto predicate = [](int element) -> bool { return (element % 2) == 0; };

    int iterations = 0;
    Algorithms::ForEachIf(container.GetBegin(), container.GetEnd(),
                          predicate,
                          [&](int element)
                          {
                              EXPECT_EQ(element, vec[iterations]);
                              ++iterations;
                          });

    EXPECT_EQ(iterations, 1);
}
