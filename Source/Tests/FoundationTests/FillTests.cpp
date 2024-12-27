#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"
#include "Foundation/Algorithms/Fill.h"

using namespace Kitsune;
using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<int>>;

TEST(FillTests, Fill)
{
    int arr[3] = { 32, 54, 76 };
    TestContainer cont(arr, arr + 3);

    Algorithms::Fill(cont.Begin, cont.End, -12);

    EXPECT_EQ(arr[0], -12);
    EXPECT_EQ(arr[1], -12);
    EXPECT_EQ(arr[2], -12);
}

TEST(FillTests, FillN)
{
    int arr[3] = { 32, 54, 76 };
    TestContainer cont(arr, arr + 3);

    Algorithms::FillN(cont.Begin, 2, -12);

    EXPECT_EQ(arr[0], -12);
    EXPECT_EQ(arr[1], -12);
    EXPECT_EQ(arr[2], 76);
}
