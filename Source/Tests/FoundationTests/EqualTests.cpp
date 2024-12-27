#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Equal.h"

namespace EqualTesting
{
    class B
    {
    public:
        B(int x) : x(x) {  }

    public:
        int x;
    };
}

using namespace Kitsune;
using namespace EqualTesting;

using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<int>>;
using BTestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<B>>;

TEST(EqualTests, OneRangeNoPred)
{
    int arr[5] = { 1, 2, 34, 45, 32 };
    int arrEq[6] = { 1, 2, 34, 45, 32, 7 };
    int arrNe[6] = { 423, 31, 342, 453, 1, 15 };

    TestContainer cont(arr, arr + 5);
    TestContainer contEqual(arrEq, arrEq + 6);
    TestContainer contNotEqual(arrNe, arrNe + 6);

    EXPECT_FALSE(Algorithms::Equal(cont.Begin, cont.End, contNotEqual.Begin));
    EXPECT_TRUE(Algorithms::Equal(cont.Begin, cont.End, contEqual.Begin));
}

TEST(EqualTests, OneRangeWithPred)
{
    B arr[5] = { 1, 2, 34, 45, 32 };
    B arrEq[6] = { 1, 2, 34, 45, 32, 7 };
    B arrNe[6] = { 423, 31, 342, 453, 1, 15 };

    BTestContainer cont(arr, arr + 5);
    BTestContainer contEqual(arrEq, arrEq + 6);
    BTestContainer contNotEqual(arrNe, arrNe + 6);

    auto pred = [](B val1, B val2) { return (val1.x == val2.x); };

    EXPECT_TRUE(Algorithms::Equal(cont.Begin, cont.End, contEqual.Begin, pred));
    EXPECT_FALSE(Algorithms::Equal(cont.Begin, cont.End, contNotEqual.Begin, pred));
}

TEST(EqualTests, TwoRangesNoPred)
{
    int arr[5] = { 1, 2, 34, 45, 32 };
    int arrEq[5] = { 1, 2, 34, 45, 32 };

    int arrNeSize[6] = { 1, 2, 34, 45, 32, 53 };
    int arrNeContents[5] = { 423, 31, 342, 453, 1 };

    TestContainer cont(arr, arr + 5);
    TestContainer contEqual(arrEq, arrEq + 5);
    TestContainer contSizeNotEqual(arrNeSize, arrNeSize + 6);
    TestContainer contContentsNotEqual(arrNeContents, arrNeContents + 5);

    EXPECT_FALSE(Algorithms::Equal(cont.Begin, cont.End, contSizeNotEqual.Begin, contSizeNotEqual.End));
    EXPECT_FALSE(Algorithms::Equal(cont.Begin, cont.End, contContentsNotEqual.Begin, contContentsNotEqual.End));

    EXPECT_TRUE(Algorithms::Equal(cont.Begin, cont.End, contEqual.Begin, contEqual.End));
}

TEST(EqualTests, TwoRangesWithPred)
{
    B arr[5] = { 1, 2, 34, 45, 32 };
    B arrEq[5] = { 1, 2, 34, 45, 32 };

    B arrNeSize[6] = { 1, 2, 34, 45, 32, 53 };
    B arrNeContents[5] = { 423, 31, 342, 453, 1 };

    BTestContainer cont(arr, arr + 5);
    BTestContainer contEqual(arrEq, arrEq + 5);
    BTestContainer contSizeNotEqual(arrNeSize, arrNeSize + 6);
    BTestContainer contContentsNotEqual(arrNeContents, arrNeContents + 5);

    auto pred = [](B val1, B val2)
    {
        return (val1.x == val2.x);
    };

    EXPECT_FALSE(Algorithms::Equal(cont.Begin, cont.End, contSizeNotEqual.Begin, contSizeNotEqual.End, pred));
    EXPECT_FALSE(Algorithms::Equal(cont.Begin, cont.End, contContentsNotEqual.Begin, contContentsNotEqual.End, pred));

    EXPECT_TRUE(Algorithms::Equal(cont.Begin, cont.End, contEqual.Begin, contEqual.End, pred));
}
