#include <gtest/gtest.h>

#include "IteratorWrappers.h"
#include "Foundation/Algorithms/Distance.h"

using namespace Kitsune;
using Testing::ForwardIteratorWrapper;

TEST(DistanceTests, NotRandomAccess)
{
    int arr[10] = { 1, 2, 3, 4, 5, 6, 1, 3, 5, 2 };
    ForwardIteratorWrapper<int> it1(arr);
    ForwardIteratorWrapper<int> it2(arr + 10);

    EXPECT_EQ(Algorithms::Distance(it1, it2), 10);
}

TEST(DistanceTests, RandomAccess)
{
    int arr[8] = { 43, 6, 12, 5, 2, 65, 1, 65 };
    int* it1 = arr;
    int* it2 = arr + 8;

    EXPECT_EQ(Algorithms::Distance(it1, it2), 8);
}
