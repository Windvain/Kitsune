#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Copy.h"

using namespace Kitsune;
using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<int>>;

TEST(CopyTests, Copy)
{
    int arr[5] = { 2, 3, 1, 4, 6 };
    int destArr[5] = { 54, 1, 2, 6, 3 };

    TestContainer cont(arr, arr + 5);
    TestContainer dest(destArr, destArr + 5);

    auto it = Algorithms::Copy(cont.Begin, cont.End, dest.Begin);
    EXPECT_EQ(it, dest.End);

    EXPECT_EQ(destArr[0], 2);
    EXPECT_EQ(destArr[1], 3);
    EXPECT_EQ(destArr[2], 1);
    EXPECT_EQ(destArr[3], 4);
    EXPECT_EQ(destArr[4], 6);
}

TEST(CopyTests, CopyN)
{
    int arr[5] = { 2, 3, 1, 4, 6 };
    int destArr[5] = { 54, 1, 2, 6, 3 };

    TestContainer cont(arr, arr + 5);
    TestContainer dest(destArr, destArr + 5);

    auto it = Algorithms::CopyN(cont.Begin, 5, dest.Begin);
    EXPECT_EQ(it, dest.End);

    EXPECT_EQ(destArr[0], 2);
    EXPECT_EQ(destArr[1], 3);
    EXPECT_EQ(destArr[2], 1);
    EXPECT_EQ(destArr[3], 4);
    EXPECT_EQ(destArr[4], 6);
}
