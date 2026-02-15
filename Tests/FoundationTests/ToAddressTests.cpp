#include "Foundation/Iterators/ToAddress.h"

#include <gtest/gtest.h>
#include "TestIterators.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

TEST(ToAddressTests, Pointer)
{
    int myInteger = 10;
    EXPECT_EQ(ToAddress(&myInteger), &myInteger);
}

TEST(ToAddressTests, Iterator)
{
    int myInteger = 10;
    ForwardIterator<int> iter(&myInteger);

    EXPECT_EQ(ToAddress(iter), &myInteger);
}
