#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Find.h"

using namespace Kitsune;
using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<int>>;

TEST(FindTests, FindByEquality)
{
    int arr[5] = { 1, 2, 54, 2, 1 };
    TestContainer cont(arr, arr + 5);

    auto firstOccurrence = Testing::ForwardIteratorWrapper<int>(arr + 1);

    EXPECT_EQ(Algorithms::Find(cont.Begin, cont.End, 2), firstOccurrence);
    EXPECT_EQ(Algorithms::Find(cont.Begin, cont.End, -1), cont.End);
}

TEST(FindTests, FindRangeByEquality)
{
    int arr[5] = { 1, 2, 54, 2, 1 };
    int findDefiniteArr[2] = { 54, 2 };
    int findImpossibleArr[3] = { 2, 3, 4 };

    TestContainer cont(arr, arr + 5);
    TestContainer willFind(findDefiniteArr, findDefiniteArr + 2);
    TestContainer wontFind(findImpossibleArr, findImpossibleArr + 3);

    auto firstOcc = Testing::ForwardIteratorWrapper<int>(arr + 2);

    EXPECT_EQ(Algorithms::Find(cont.Begin, cont.End, willFind.Begin, willFind.End), firstOcc);
    EXPECT_EQ(Algorithms::Find(cont.Begin, cont.End, wontFind.Begin, wontFind.End), cont.End);
}

TEST(FindTests, FindIf)
{
    int arr[5] = { 1, 2, 54, 2, 1 };
    TestContainer cont(arr, arr + 5);

    auto firstOccurrence = Testing::ForwardIteratorWrapper<int>(arr + 1);

    EXPECT_EQ(Algorithms::FindIf(cont.Begin, cont.End, [](int x) -> bool { return x == 2; }), firstOccurrence);
    EXPECT_EQ(Algorithms::FindIf(cont.Begin, cont.End, [](int x) -> bool { return x == -1; }), cont.End);
}

TEST(FindTests, FindLastIf)
{
    int arr[5] = { 1, 2, 54, 2, 1 };
    TestContainer cont(arr, arr + 5);

    auto last = Testing::ForwardIteratorWrapper<int>(arr + 4);
    auto pred = [](int x) -> bool { return (x % 2) != 0; };

    EXPECT_EQ(Algorithms::FindLastIf(cont.Begin, cont.End, pred), last);
}

TEST(FindTests, FindLast)
{
    int arr[5] = { 1, 2, 54, 2, 1 };
    TestContainer cont(arr, arr + 5);

    auto last = Testing::ForwardIteratorWrapper<int>(arr + 3);

    EXPECT_EQ(Algorithms::FindLast(cont.Begin, cont.End, 2), last);
    EXPECT_EQ(Algorithms::FindLast(cont.Begin, cont.End, -1), cont.End);
}
