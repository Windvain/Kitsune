#include <gtest/gtest.h>
#include "TestIterators.h"

#include "Foundation/Iterators/ToAddress.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardIterator;

    // ToAddress(T*)
    TEST(ToAddressTests, Pointer)
    {
        int myInteger = 10;
        EXPECT_EQ(ToAddress(&myInteger), &myInteger);
    }

    // ToAddress(Iter)
    TEST(ToAddressTests, Iterator)
    {
        int myInteger = 10;
        ForwardIterator<int> iter(&myInteger);

        EXPECT_EQ(ToAddress(iter), &myInteger);
    }
}
