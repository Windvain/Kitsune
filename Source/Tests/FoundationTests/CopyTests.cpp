#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Copy.h"

using namespace Kitsune;

using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<int>>;
using BTestContainer = Testing::TestContainer<Testing::BidirIteratorWrapper<int>>;

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

TEST(CopyTests, CopyIf)
{
    int arr[5] = { 2, 3, 1, 4, 6 };
    int destArr[5] = { 54, 1, 2, 6, 3 };

    TestContainer cont(arr, arr + 5);
    TestContainer dest(destArr, destArr + 5);

    auto it = Algorithms::CopyIf(cont.Begin, cont.End, dest.Begin,
        [](int x) -> bool
        {
            return (x % 2) == 0;
        });

    EXPECT_EQ(it, Testing::ForwardIteratorWrapper(dest.Begin.Pointer() + 3));
    EXPECT_EQ(destArr[0], 2);
    EXPECT_EQ(destArr[1], 4);
    EXPECT_EQ(destArr[2], 6);
    EXPECT_EQ(destArr[3], 6);
    EXPECT_EQ(destArr[4], 3);
}

TEST(CopyTests, CopyBackwards)
{
    int arr[5] = { 2, 3, 1, 4, 6 };
    int destArr[5] = { 54, 1, 2, 6, 3 };

    BTestContainer cont(arr, arr + 5);
    BTestContainer dest(destArr, destArr + 5);

    auto it = Algorithms::CopyBackwards(cont.Begin, cont.End, dest.End);
    EXPECT_EQ(it, dest.Begin);

    EXPECT_EQ(destArr[0], 2);
    EXPECT_EQ(destArr[1], 3);
    EXPECT_EQ(destArr[2], 1);
    EXPECT_EQ(destArr[3], 4);
    EXPECT_EQ(destArr[4], 6);
}
