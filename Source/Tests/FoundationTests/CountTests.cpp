#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Algorithms/Count.h"
#include "Foundation/Common/Macros.h"

template<typename T>
using ForwardContainer = Testing::TestContainer<Testing::ForwardIteratorWrapper<T>>;

using namespace Kitsune;

TEST(CountTests, Count)
{
    int arr[] = { 3, 4, 11, 6, 23, 11, 12, 11 };

    ForwardContainer<int> cont(arr, arr + KITSUNE_ARRAY_SIZE(arr));
    std::ptrdiff_t cnt = Algorithms::Count(cont.Begin, cont.End, 11);

    EXPECT_EQ(cnt, 3);
}

TEST(CountTests, CountIf)
{
    int arr[] = { 3, 4, 11, 6, 23, 11, 12, 11 };

    ForwardContainer<int> cont(arr, arr + KITSUNE_ARRAY_SIZE(arr));
    std::ptrdiff_t cnt = Algorithms::CountIf(cont.Begin, cont.End,
        [](int elem) -> bool { return (elem % 2) != 0; });

    EXPECT_EQ(cnt, 5);
}
