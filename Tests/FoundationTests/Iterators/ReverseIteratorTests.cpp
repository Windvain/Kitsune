#include <gtest/gtest.h>
#include "TestIterators.h"

#include "Foundation/Iterators/ReverseIterator.h"

namespace
{
    using namespace Kitsune;
    using Testing::BidirectionalIterator;

    template<typename T>
    using Bidir = Testing::BidirectionalIterator<T>;

    template<typename T>
    using RandAccess = Testing::RandomAccessIterator<T>;

    // ReverseIterator<Iter>::ReverseIterator()
    TEST(ReverseIteratorTest, DefaultConstructor)
    {
        ReverseIterator<Bidir<int>> iterator{};
        EXPECT_EQ(iterator.GetBase().Pointer(), nullptr);
    }

    // ReverseIterator<Iter>::ReverseIterator(Iter iter)
    TEST(ReverseIteratorTest, IteratorConstructor)
    {
        int value = 348;
        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(&value) };

        EXPECT_EQ(iterator.GetBase().Pointer(), &value);
    }

    // ReverseIterator<Iter>::ReverseIterator(const ReverseIterator&)
    TEST(ReverseIteratorTest, CopyConstructor)
    {
        int value = 485;
        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(&value) };

        ASSERT_EQ(iterator.GetBase().Pointer(), &value);
        ReverseIterator<Bidir<int>> copy = iterator;

        EXPECT_EQ(copy.GetBase().Pointer(), &value);
    }

    // ReverseIterator<Iter>::ReverseIterator(const ReverseIterator<Iter2>&)
    TEST(ReverseIteratorTest, CopyConstructorWithCast)
    {
        int value = 485;
        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(&value) };

        ASSERT_EQ(iterator.GetBase().Pointer(), &value);
        ReverseIterator<int*> copy = iterator;

        EXPECT_EQ(copy.GetBase(), &value);
    }

    // ReverseIterator<Iter>::operator=(const ReverseIterator&)
    TEST(ReverseIteratorTest, CopyAssign)
    {
        int value = 485;
        int value2 = 834;

        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(&value) };
        ASSERT_EQ(iterator.GetBase().Pointer(), &value);

        ReverseIterator<Bidir<int>> copy{ Bidir<int>(&value2) };
        copy = iterator;

        EXPECT_EQ(copy.GetBase().Pointer(), &value);
    }

    // ReverseIterator<Iter>::operator=(const ReverseIterator<Iter2>&)
    TEST(ReverseIteratorTest, CopyAssignWithCast)
    {
        int value = 485;
        int value2 = 521;

        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(&value) };
        ASSERT_EQ(iterator.GetBase().Pointer(), &value);

        ReverseIterator<int*> copy{ Bidir<int>(&value2) };
        copy = iterator;

        EXPECT_EQ(copy.GetBase(), &value);
    }

    // ReverseIterator<Iter>::operator*()
    TEST(ReverseIteratorTest, Dereference)
    {
        int array[2] = { 4, 21 };
        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(array + 1) };

        EXPECT_EQ(&(*iterator), &array[0]);
    }

    // ReverseIterator<Iter>::operator->()
    TEST(ReverseIteratorTest, ArrowOperator)
    {
        std::string array[2] = { "Hello, ", "World!" };
        ReverseIterator<Bidir<std::string>> iterator{ Bidir<std::string>(array + 1) };

        EXPECT_EQ(iterator->size(), 7);
        EXPECT_EQ(iterator.operator->(), &array[0]);
    }

    // ReverseIterator<Iter>::operator[](Index)
    TEST(ReverseIteratorTest, Subscript)
    {
        int array[2] = { 4, 12 };
        ReverseIterator<RandAccess<int>> iterator{ RandAccess<int>(array + 2) };

        EXPECT_EQ(&iterator[0], &array[1]);
        EXPECT_EQ(&iterator[1], &array[0]);
    }

    // ReverseIterator<Iter>::operator++()
    // ReverseIterator<Iter>::operator++(int)
    TEST(ReverseIteratorTest, Increment)
    {
        int array[3] = { 31, 12, 45 };
        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(array + 3) };

        EXPECT_EQ((iterator++).GetBase().Pointer(), array + 3);
        EXPECT_EQ(iterator.GetBase().Pointer(), array + 2);

        EXPECT_EQ((++iterator).GetBase().Pointer(), array + 1);
        EXPECT_EQ(iterator.GetBase().Pointer(), array + 1);
    }

    // ReverseIterator<Iter>::operator--()
    // ReverseIterator<Iter>::operator--(int)
    TEST(ReverseIteratorTest, Decrement)
    {
        int array[3] = { 31, 12, 45 };
        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(array) };

        EXPECT_EQ((iterator--).GetBase().Pointer(), array);
        EXPECT_EQ(iterator.GetBase().Pointer(), array + 1);

        EXPECT_EQ((--iterator).GetBase().Pointer(), array + 2);
        EXPECT_EQ(iterator.GetBase().Pointer(), array + 2);
    }

    // ReverseIterator<Iter>::operator+(Ptrdiff)
    // ReverseIterator<Iter>::operator+=(Ptrdiff)
    // operator+(Ptrdiff, const ReverseIterator<Iter>&)
    TEST(ReverseIteratorTest, AddOperator)
    {
        int array[3] = { 32, 56, 1 };
        ReverseIterator<RandAccess<int>> iterator{ RandAccess<int>(array + 3) };

        EXPECT_EQ((iterator + 2).GetBase().Pointer(), array + 1);
        EXPECT_EQ((2 + iterator).GetBase().Pointer(), array + 1);
        EXPECT_EQ(iterator.GetBase().Pointer(), array + 3);

        iterator += 2;
        EXPECT_EQ(iterator.GetBase().Pointer(), array + 1);
    }

    // ReverseIterator<Iter>::operator-(Ptrdiff)
    // ReverseIterator<Iter>::operator-=(Ptrdiff)
    // operator-(const ReverseIterator&, const ReverseIterator&)
    TEST(ReverseIteratorTest, SubtractOperator)
    {
        int array[3] = { 32, 56, 1 };
        ReverseIterator<RandAccess<int>> iterator{ RandAccess<int>(array) };

        EXPECT_EQ((iterator - 2).GetBase().Pointer(), array + 2);
        EXPECT_EQ(iterator.GetBase().Pointer(), array);

        iterator -= 2;
        EXPECT_EQ(iterator.GetBase().Pointer(), array + 2);

        ReverseIterator<RandAccess<int>> begin{ RandAccess<int>(array) };
        EXPECT_EQ(begin - iterator, 2);
    }

    /* GetBase() is assumed to work. */

    // operator==(const ReverseIterator<Iter>&, const ReverseIterator<Iter>&)
    // operator!=(const ReverseIterator<Iter>&, const ReverseIterator<Iter>&)
    TEST(ReverseIteratorTest, EqualityComparison)
    {
        int array[3] = { 123, 56, 1 };
        ReverseIterator<Bidir<int>> iterator{ Bidir<int>(array + 1) };

        ReverseIterator<Bidir<int>> sameIterator{ Bidir<int>(array + 1) };
        ReverseIterator<Bidir<int>> diffIterator{ Bidir<int>(array + 2) };

        EXPECT_EQ(iterator, sameIterator);
        EXPECT_NE(iterator, diffIterator);
    }

    // operator>=(const ReverseIterator<Iter>&, const ReverseIterator<Iter>&)
    // operator<=(const ReverseIterator<Iter>&, const ReverseIterator<Iter>&)
    // operator>(const ReverseIterator<Iter>&, const ReverseIterator<Iter>&)
    // operator<(const ReverseIterator<Iter>&, const ReverseIterator<Iter>&)
    TEST(ReverseIteratorTest, Comparison)
    {
        int array[3] = { 123, 56, 1 };
        ReverseIterator<RandAccess<int>> iterator{ RandAccess<int>(array + 1) };

        ReverseIterator<RandAccess<int>> lessIterator{ RandAccess<int>(array) };
        ReverseIterator<RandAccess<int>> moreIterator{ RandAccess<int>(array + 2) };

        EXPECT_LE(moreIterator, iterator);
        EXPECT_LE(moreIterator, lessIterator);
        EXPECT_LE(iterator, lessIterator);
        EXPECT_LE(iterator, iterator);

        EXPECT_GE(lessIterator, iterator);
        EXPECT_GE(lessIterator, moreIterator);
        EXPECT_GE(iterator, moreIterator);
        EXPECT_GE(iterator, iterator);

        EXPECT_LT(moreIterator, iterator);
        EXPECT_LT(moreIterator, lessIterator);
        EXPECT_LT(iterator, lessIterator);

        EXPECT_GT(lessIterator, iterator);
        EXPECT_GT(lessIterator, moreIterator);
        EXPECT_GT(iterator, moreIterator);
    }
}
