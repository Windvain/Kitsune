#include <gtest/gtest.h>
#include "TestIterators.h"

#include "Foundation/Algorithms/Advance.h"

namespace
{
    using namespace Kitsune;

    template<typename T>
    class AdvanceTest : public ::testing::Test
    {
    protected:
        using IteratorType = T;
        using ValueType = typename IteratorTraits<T>::ValueType;
    };

    using AdvanceTestTypes =
        ::testing::Types<
            Testing::ForwardIterator<int>,
            Testing::BidirectionalIterator<float>,
            Testing::RandomAccessIterator<long>>;

    TYPED_TEST_SUITE(AdvanceTest, AdvanceTestTypes);

    // Algorithms::Advance(Iter&, DifferenceType(x > 0))
    TYPED_TEST(AdvanceTest, AdvanceForwards)
    {
        using ValueType = typename TestFixture::ValueType;
        using IteratorType = typename TestFixture::IteratorType;

        auto* rawPointer = new ValueType();
        IteratorType iterator(rawPointer);

        Algorithms::Advance(iterator, 5);
        EXPECT_EQ(iterator.Pointer(), rawPointer + 5);

        delete rawPointer;
    }

    // Algorithms::Advance(Iter&, DifferenceType(x < 0))
    TYPED_TEST(AdvanceTest, AdvanceBackwards)
    {
        using ValueType = typename TestFixture::ValueType;
        using IteratorType = typename TestFixture::IteratorType;

        if constexpr (!ForwardIterator<IteratorType>)
        {
            auto* rawPointer = new ValueType();
            IteratorType iterator(rawPointer);

            Algorithms::Advance(iterator, -5);
            EXPECT_EQ(iterator.Pointer(), rawPointer - 5);

            delete rawPointer;
        }
    }

    // Algorithms::Advance(Iter&, 0)
    TYPED_TEST(AdvanceTest, AdvanceZero)
    {
        using ValueType = typename TestFixture::ValueType;
        using IteratorType = typename TestFixture::IteratorType;

        auto* rawPointer = new ValueType();
        IteratorType iterator(rawPointer);

        Algorithms::Advance(iterator, 0);
        EXPECT_EQ(iterator.Pointer(), rawPointer);

        delete rawPointer;
    }
}
