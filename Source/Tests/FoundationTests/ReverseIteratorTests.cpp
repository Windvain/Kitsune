#include <gtest/gtest.h>

#include "IteratorWrappers.h"
#include "Foundation/Iterators/ReverseIterator.h"

using namespace Kitsune;
using Testing::BidirIteratorWrapper;

namespace ReverseIteratorTesting
{
    class B { /* ... */ };
    class C : public B
    {
    };
}

using namespace ReverseIteratorTesting;

TEST(ReverseIteratorTests, DefaultCtor)
{
    int x;
    BidirIteratorWrapper<int> it(&x);

    auto rev = ReverseIterator<BidirIteratorWrapper<int>>(it);
    EXPECT_EQ(&*rev.GetBase(), &x);
}

TEST(ReverseIteratorTests, IteratorCtor)
{
    int x;
    auto it = BidirIteratorWrapper<int>(&x);

    auto rev = ReverseIterator<decltype(it)>(it);
    EXPECT_EQ(rev.GetBase(), it);
}

TEST(ReverseIteratorTests, TemplatedCopyCtor)
{
    C x;
    auto rev = ReverseIterator<C*>(&x);
    ReverseIterator<B*> base = rev;

    EXPECT_EQ(rev.GetBase(), base.GetBase());
}

TEST(ReverseIteratorTests, TemplatedCopyAssign)
{
    C x;
    auto rev = ReverseIterator<C*>(&x);
    ReverseIterator<B*> base = ReverseIterator<B*>();

    base = rev;

    EXPECT_EQ(rev.GetBase(), base.GetBase());
}

TEST(ReverseIteratorTests, Dereference)
{
    int x;
    auto it = BidirIteratorWrapper<int>(&x + 1);

    auto rev = ReverseIterator<decltype(it)>(it);
    EXPECT_EQ(&*rev, &x);
}

TEST(ReverseIteratorTests, ArrowOperator)
{
    int x = 5;

    auto it = BidirIteratorWrapper<int>(&x + 1);
    auto rev = ReverseIterator<BidirIteratorWrapper<int>>(it);

    EXPECT_EQ(rev.operator->(), &x);
}

TEST(ReverseIteratorTests, PreIncrement)
{
    int x[2];

    auto it = BidirIteratorWrapper<int>(x + 2);
    auto rev = ReverseIterator<BidirIteratorWrapper<int>>(it);

    ++rev;

    EXPECT_EQ(&*rev, x);
}

TEST(ReverseIteratorTests, PostIncrement)
{
    int x[2];

    auto it = BidirIteratorWrapper<int>(x + 2);
    auto rev = ReverseIterator<BidirIteratorWrapper<int>>(it);

    auto first = rev++;

    EXPECT_EQ(first.operator->(), x + 1);
    EXPECT_EQ(&*rev, x);
}

TEST(ReverseIteratorTests, PreDecrement)
{
    int x[2];

    auto it = BidirIteratorWrapper<int>(x + 1);
    auto rev = ReverseIterator<BidirIteratorWrapper<int>>(it);

    --rev;

    EXPECT_EQ(&*rev, x + 1);
}

TEST(ReverseIteratorTests, PostDecrement)
{
    int x[2];

    auto it = BidirIteratorWrapper<int>(x + 1);
    auto rev = ReverseIterator<BidirIteratorWrapper<int>>(it);

    auto last = rev--;
    EXPECT_EQ(last.operator->(), x);
    EXPECT_EQ(&*rev, x + 1);
}

TEST(ReverseIteratorTests, AdvanceOperator)
{
    int x[5];

    auto it = ReverseIterator<int*>(x + 4);
    auto it2 = it + 2;

    EXPECT_EQ(&*it2, x + 1);
}

TEST(ReverseIteratorTests, SubtractOperator)
{
    int x[5];

    auto it = ReverseIterator<int*>(x + 1);
    auto it2 = it - 2;

    EXPECT_EQ(&*it2, x + 2);
}

TEST(ReverseIteratorTests, AdvanceAssign)
{
    int x[5];
    auto it = ReverseIterator<int*>(x + 4);

    it += 2;

    EXPECT_EQ(&*it, x + 1);
}

TEST(ReverseIteratorTests, SubtractAssign)
{
    int x[5];
    auto it = ReverseIterator<int*>(x + 2);

    it -= 3;

    EXPECT_EQ(&*it, x + 4);
}

TEST(ReverseIteratorTests, EqualOperator)
{
    BidirIteratorWrapper<int> x((int*)5);
    auto it = ReverseIterator<BidirIteratorWrapper<int>>(x);

    EXPECT_TRUE(it == ReverseIterator<BidirIteratorWrapper<int>>(x));
}

TEST(ReverseIteratorTests, NotEqualOperator)
{
    BidirIteratorWrapper<int> x((int*)5);
    auto it = ReverseIterator<BidirIteratorWrapper<int>>(x);

    EXPECT_FALSE(it != ReverseIterator<BidirIteratorWrapper<int>>(x));
}

TEST(ReverseIteratorTests, GreaterEqualOperator)
{
    auto it = ReverseIterator<char*>((char*)123123);
    EXPECT_FALSE(it >= ReverseIterator<char*>((char*)123121));

    EXPECT_TRUE(it >= ReverseIterator<char*>((char*)123123));
    EXPECT_TRUE(it >= ReverseIterator<char*>((char*)9210923));
}

TEST(ReverseIteratorTests, LessEqualOperator)
{
    auto it = ReverseIterator<char*>((char*)123123);
    EXPECT_FALSE(it <= ReverseIterator<char*>((char*)9210923));

    EXPECT_TRUE(it <= ReverseIterator<char*>((char*)123123));
    EXPECT_TRUE(it <= ReverseIterator<char*>((char*)123121));
}

TEST(ReverseIteratorTests, GreaterThanOperator)
{
    auto it = ReverseIterator<char*>((char*)123123);
    EXPECT_TRUE(it > ReverseIterator<char*>((char*)9210923));

    EXPECT_FALSE(it > ReverseIterator<char*>((char*)123123));
    EXPECT_FALSE(it > ReverseIterator<char*>((char*)123121));
}

TEST(ReverseIteratorTests, LessThanOperator)
{
    auto it = ReverseIterator<char*>((char*)123123);
    EXPECT_TRUE(it < ReverseIterator<char*>((char*)123121));

    EXPECT_FALSE(it < ReverseIterator<char*>((char*)123123));
    EXPECT_FALSE(it < ReverseIterator<char*>((char*)9210923));
}
