#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Swap.h"

using namespace Kitsune;
using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<int>>;

TEST(SwapTests, Swap)
{
    int x = 3;
    int y = 5;

    Algorithms::Swap(x, y);

    EXPECT_EQ(x, 5);
    EXPECT_EQ(y, 3);
}

TEST(SwapTests, SwapRanges)
{
    int arr1[3] = { 0, 1, 2 };
    int arr2[3] = { 6, 5, 4 };

    TestContainer cont1(arr1, arr1 + 3);
    TestContainer cont2(arr2, arr2 + 3);

    auto end = Algorithms::Swap(cont1.Begin, cont1.End, cont2.Begin);
    EXPECT_EQ(end, cont2.End);

    EXPECT_EQ(arr1[0], 6);
    EXPECT_EQ(arr1[1], 5);
    EXPECT_EQ(arr1[2], 4);

    EXPECT_EQ(arr2[0], 0);
    EXPECT_EQ(arr2[1], 1);
    EXPECT_EQ(arr2[2], 2);
}

TEST(SwapTests, IteratorSwap)
{
    int arr[3] = { 0, 1, 2 };
    TestContainer cont(arr, arr + 3);

    auto with = cont.Begin;
    Algorithms::IteratorSwap(cont.Begin, ++with);

    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 0);
    EXPECT_EQ(arr[2], 2);
}
