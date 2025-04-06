#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/ForEach.h"

using namespace Kitsune;
using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<int>>;

TEST(ForEachTests, ForEach)
{
    int arr[5] = { 1, 2, 3, 4, 5 };
    TestContainer cont(arr, arr + 5);

    int n = 0;
    Algorithms::ForEach(cont.Begin, cont.End, [&](int x)
    {
        EXPECT_EQ(n + 1, x);
        ++n;
    });

    EXPECT_EQ(n, 5);
}

TEST(ForEachTests, ForEachN)
{
    int arr[5] = { 1, 2, 3, 4, 5 };
    TestContainer cont(arr, arr + 5);

    int n = 0;
    int k = 0;
    Algorithms::ForEachN(cont.Begin, 5, [&](int x)
    {
        EXPECT_EQ(k + 1, x);
        n += x;
        ++k;
    });

    EXPECT_EQ(n, 15);
}


TEST(ForEachTests, ForEachIf)
{
    int arr[5] = { 1, 2, 75, 2, 2 };
    TestContainer cont(arr, arr + 5);

    int n = 0;
    auto pred = [](int x) -> bool { return x == 2; };

    Algorithms::ForEachIf(cont.Begin, cont.End, pred,
        [&](int x) { EXPECT_EQ(x, 2); ++n; });

    EXPECT_EQ(n, 3);
}
