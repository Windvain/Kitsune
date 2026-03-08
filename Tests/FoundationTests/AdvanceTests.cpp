#include "Foundation/Algorithms/Advance.h"

#include <gtest/gtest.h>
#include "TestIterators.h"

using namespace Kitsune;

template<typename T>
class AdvanceTests : public ::testing::Test
{
protected:
    using IteratorType = T;
    using ValueType = typename IteratorTraits<T>::ValueType;

protected:
    AdvanceTests() { /* ... */ }
    ~AdvanceTests() { /* ... */ }
};

using AdvanceTestsImpl =
    ::testing::Types<
        Testing::ForwardIterator<int>,
        Testing::BidirectionalIterator<float>,
        Testing::RandomAccessIterator<long>>;

TYPED_TEST_SUITE(AdvanceTests, AdvanceTestsImpl);

TYPED_TEST(AdvanceTests, Advance)
{
    using ValueType = typename TestFixture::ValueType;
    using IteratorType = typename TestFixture::IteratorType;

    auto* rawPointer = new ValueType();
    IteratorType iterator(rawPointer);

    Algorithms::Advance(iterator, 5);

    EXPECT_EQ(iterator.Pointer(), rawPointer + 5);
    delete rawPointer;
}
