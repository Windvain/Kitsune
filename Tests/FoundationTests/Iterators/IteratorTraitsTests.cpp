#include <gtest/gtest.h>

#include "Foundation/Common/Macros.h"
#include "Foundation/Iterators/IteratorTraits.h"

namespace
{
    using namespace Kitsune;

    template<typename T>
    class MyIterator
    {
    public:
        using ValueType = T**;          // Double pointer! >:)
        using DifferenceType = short;   // Smooollll

    public:
        MyIterator() = default;
        ~MyIterator() = default;

        MyIterator(const MyIterator&) = default;
        MyIterator& operator=(const MyIterator&) = default;

    public:
        inline MyIterator& operator++()
        {
            return *this;
        }

        inline MyIterator operator++(int)
        {
            return *this;
        }

    public:
        inline T& operator*() const { KITSUNE_UNREACHABLE(); }
        inline T* operator->() const { return nullptr; }

    public:
        bool operator==(const MyIterator& iter) const
        {
            return true;
        }
    };

    // IteratorTraits<T*>::ValueType & IteratorTraits<T*>::DifferenceType
    TEST(IteratorTraitsTest, Pointer)
    {
        EXPECT_TRUE((std::is_same_v<typename IteratorTraits<int*>::ValueType, int>));
        EXPECT_TRUE((std::is_same_v<
            typename IteratorTraits<const int*>::ValueType,
            const int>));

        EXPECT_TRUE((std::is_same_v<
            typename IteratorTraits<int*>::DifferenceType,
            Ptrdiff>));

        EXPECT_TRUE((std::is_same_v<
            typename IteratorTraits<const int*>::DifferenceType,
            Ptrdiff>));
    }

    // IteratorTraits<T>::ValueType & IteratorTraits<T>::DifferenceType
    TEST(IteratorTraitsTest, CustomIterators)
    {
        EXPECT_TRUE((std::is_same_v<
            typename IteratorTraits<MyIterator<float>>::ValueType,
            float**>));

        EXPECT_TRUE((std::is_same_v<
            typename IteratorTraits<MyIterator<const int>>::ValueType,
            const int**>));

        EXPECT_TRUE((std::is_same_v<
            typename IteratorTraits<MyIterator<int>>::DifferenceType,
            short>));

        EXPECT_TRUE((std::is_same_v<
            typename IteratorTraits<MyIterator<const int*>>::DifferenceType,
            short>));
    }
}
