#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Move.h"

using namespace Kitsune;

namespace MoveTesting
{
    class A
    {
    public:
        A() = default;
        A(int x) : Value(x) { /* ... */ }
        A(const A& a) : Value(a.Value) { /* ... */ }

        A(A&& a) : Value(a.Value)
        {
            a.Value = 0;
        }

    public:
        A& operator=(const A& a) { Value = a.Value; return *this; }
        A& operator=(A&& a) { Value = a.Value; a.Value = 0; return *this; }

    public:
        int Value;
    };
}

using namespace MoveTesting;
using TestContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<A>>;

TEST(MoveTests, Move)
{
    A arr[5] = { 2, 3, 1, 4, 6 };
    A destArr[5] = { 54, 1, 2, 6, 3 };

    TestContainer cont(arr, arr + 5);
    TestContainer dest(destArr, destArr + 5);

    auto it = Algorithms::Move(cont.Begin, cont.End, dest.Begin);
    EXPECT_EQ(it, dest.End);

    EXPECT_EQ(arr[0].Value, 0);
    EXPECT_EQ(arr[1].Value, 0);
    EXPECT_EQ(arr[2].Value, 0);
    EXPECT_EQ(arr[3].Value, 0);
    EXPECT_EQ(arr[4].Value, 0);

    EXPECT_EQ(destArr[0].Value, 2);
    EXPECT_EQ(destArr[1].Value, 3);
    EXPECT_EQ(destArr[2].Value, 1);
    EXPECT_EQ(destArr[3].Value, 4);
    EXPECT_EQ(destArr[4].Value, 6);
}

TEST(MoveTests, MoveN)
{
    A arr[5] = { 2, 3, 1, 4, 6 };
    A destArr[5] = { 54, 1, 2, 6, 3 };

    TestContainer cont(arr, arr + 5);
    TestContainer dest(destArr, destArr + 5);

    auto it = Algorithms::MoveN(cont.Begin, 5, dest.Begin);
    EXPECT_EQ(it, dest.End);

    EXPECT_EQ(arr[0].Value, 0);
    EXPECT_EQ(arr[1].Value, 0);
    EXPECT_EQ(arr[2].Value, 0);
    EXPECT_EQ(arr[3].Value, 0);
    EXPECT_EQ(arr[4].Value, 0);

    EXPECT_EQ(destArr[0].Value, 2);
    EXPECT_EQ(destArr[1].Value, 3);
    EXPECT_EQ(destArr[2].Value, 1);
    EXPECT_EQ(destArr[3].Value, 4);
    EXPECT_EQ(destArr[4].Value, 6);
}
