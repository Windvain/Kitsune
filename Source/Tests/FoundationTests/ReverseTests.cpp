#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Reverse.h"

using namespace Kitsune;

using BidirTestCont = Testing::TestContainer<Testing::BidirIteratorWrapper<int>>;
using RATestCont = Testing::TestContainer<Testing::RandomAccessIteratorWrapper<int>>;

TEST(ReverseTests, BidirIterator)
{
    int arr[6] = { 3, 4, 2, 65, 1, 22 };
    BidirTestCont cont(arr, arr + 6);

    Algorithms::Reverse(cont.Begin, cont.End);

    EXPECT_EQ(arr[0], 22);
    EXPECT_EQ(arr[1], 1);
    EXPECT_EQ(arr[2], 65);
    EXPECT_EQ(arr[3], 2);
    EXPECT_EQ(arr[4], 4);
    EXPECT_EQ(arr[5], 3);
}

TEST(ReverseTests, RAIterator)
{
    int arr[6] = { 3, 4, 2, 65, 1, 22 };
    RATestCont cont(arr, arr + 6);

    Algorithms::Reverse(cont.Begin, cont.End);

    EXPECT_EQ(arr[0], 22);
    EXPECT_EQ(arr[1], 1);
    EXPECT_EQ(arr[2], 65);
    EXPECT_EQ(arr[3], 2);
    EXPECT_EQ(arr[4], 4);
    EXPECT_EQ(arr[5], 3);
}
