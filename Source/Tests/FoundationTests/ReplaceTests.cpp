#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Replace.h"

template<typename T>
using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<T>>;

using namespace Kitsune;

TEST(ReplaceTests, Replace)
{
    int arr[5] = { 4, 5, 6, 4, 2 };
    TestContainer<int> cont(arr, arr + 5);

    Algorithms::Replace(cont.Begin, cont.End, 4, 17);
    EXPECT_EQ(arr[0], 17);
    EXPECT_EQ(arr[1], 5);
    EXPECT_EQ(arr[2], 6);
    EXPECT_EQ(arr[3], 17);
    EXPECT_EQ(arr[4], 2);
}

TEST(ReplaceTests, ReplaceN)
{
    int arr[5] = { 4, 5, 6, 4, 2 };
    TestContainer<int> cont(arr, arr + 5);

    Algorithms::ReplaceN(cont.Begin, 5, 4, 17);
    EXPECT_EQ(arr[0], 17);
    EXPECT_EQ(arr[1], 5);
    EXPECT_EQ(arr[2], 6);
    EXPECT_EQ(arr[3], 17);
    EXPECT_EQ(arr[4], 2);
}

TEST(ReplaceTests, ReplaceIf)
{
    int arr[5] = { 4, 5, 6, 4, 2 };
    TestContainer<int> cont(arr, arr + 5);

    const auto pred = [](int x) -> bool { return (x % 2) != 0; };

    Algorithms::ReplaceIf(cont.Begin, cont.End, pred, 17);
    EXPECT_EQ(arr[0], 4);
    EXPECT_EQ(arr[1], 17);
    EXPECT_EQ(arr[2], 6);
    EXPECT_EQ(arr[3], 4);
    EXPECT_EQ(arr[4], 2);
}
