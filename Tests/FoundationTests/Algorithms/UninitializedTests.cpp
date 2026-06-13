#include <cstring>
#include <gtest/gtest.h>

#include "TestContainer.h"
#include "Foundation/Algorithms/Advance.h"
#include "Foundation/Algorithms/Uninitialized.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardNonOwningTestContainer;

    class UninitializedTest : public ::testing::Test
    {
    protected:
        inline void SetUp() override
        {
            StringContainer = CreateContainer<std::string, 5>();
            PointerContainer = CreateContainer<std::shared_ptr<int>, 5>();
        }

        inline void TearDown() override
        {
            DestroyContainer(StringContainer);
            DestroyContainer(PointerContainer);
        }

    private:
        template<typename T, Usize S>
        inline ForwardNonOwningTestContainer<T, S> CreateContainer()
        {
            auto* pointer = static_cast<T*>(std::malloc(sizeof(T) * S));
            if (pointer == nullptr)
                throw std::bad_alloc();

            std::memset((void*)pointer, 0b01010101, S * sizeof(T));
            return ForwardNonOwningTestContainer<T, S>(pointer);
        }

        template<typename T, Usize S>
        inline void DestroyContainer(ForwardNonOwningTestContainer<T, S> container)
        {
            std::free(container.m_Array);
        }

    protected:
        ForwardNonOwningTestContainer<std::string, 5> StringContainer;
        ForwardNonOwningTestContainer<std::shared_ptr<int>, 5> PointerContainer;
    };

    // Algorithms::UninitializedCopy(Iter, Iter, OutIter)
    TEST_F(UninitializedTest, UninitializedCopy)
    {
        std::shared_ptr<int> array[5] = {
            std::make_shared<int>(10),
            std::make_shared<int>(1140),
            std::make_shared<int>(210),
            std::make_shared<int>(1560),
            std::make_shared<int>(2011),
        };

        auto iterator = Algorithms::UninitializedCopy(
            array,
            array + KITSUNE_ARRAY_SIZE(array),
            PointerContainer.GetBegin());

        EXPECT_EQ(iterator, PointerContainer.GetEnd());

        for (int index = 0; index < StringContainer.Size(); ++index)
        {
            EXPECT_EQ(PointerContainer[index], array[index]);
            EXPECT_EQ(PointerContainer[index].use_count(), 2);
        }

        std::destroy_n(PointerContainer.m_Array, PointerContainer.Size());
    }

    // Algorithms::UninitializedCopyN(Iter, Size, OutIter)
    TEST_F(UninitializedTest, UninitializedCopyN)
    {
        std::shared_ptr<int> array[5] = {
            std::make_shared<int>(10),
            std::make_shared<int>(1140),
            std::make_shared<int>(210),
            std::make_shared<int>(1560),
            std::make_shared<int>(2011),
        };

        auto iterator = Algorithms::UninitializedCopyN(
            array,
            KITSUNE_ARRAY_SIZE(array),
            PointerContainer.GetBegin());

        EXPECT_EQ(iterator, PointerContainer.GetEnd());

        for (int index = 0; index < PointerContainer.Size(); ++index)
        {
            EXPECT_EQ(PointerContainer[index], array[index]);
            EXPECT_EQ(PointerContainer[index].use_count(), 2);
        }

        std::destroy_n(PointerContainer.m_Array, PointerContainer.Size());
    }

    // Algorithms::UninitializedMove(Iter, Iter, OutIter)
    TEST_F(UninitializedTest, UninitializedMove)
    {
        int valueArray[5] = { 10, 1140, 210, 1560, 2011 };
        std::shared_ptr<int> array[5] = {
            std::make_shared<int>(10),
            std::make_shared<int>(1140),
            std::make_shared<int>(210),
            std::make_shared<int>(1560),
            std::make_shared<int>(2011),
        };

        auto iterator = Algorithms::UninitializedMove(
            array,
            array + KITSUNE_ARRAY_SIZE(array),
            PointerContainer.GetBegin());

        EXPECT_EQ(iterator, PointerContainer.GetEnd());

        for (int index = 0; index < PointerContainer.Size(); ++index)
        {
            EXPECT_EQ(*PointerContainer[index], valueArray[index]);
            EXPECT_EQ(PointerContainer[index].use_count(), 1);
            EXPECT_EQ(array[index], nullptr);
        }

        std::destroy_n(PointerContainer.m_Array, PointerContainer.Size());
    }

    // Algorithms::UninitializedMoveN(Iter, Size, OutIter)
    TEST_F(UninitializedTest, UninitializedMoveN)
    {
        int valueArray[5] = { 10, 1140, 210, 1560, 2011 };
        std::shared_ptr<int> array[5] = {
            std::make_shared<int>(10),
            std::make_shared<int>(1140),
            std::make_shared<int>(210),
            std::make_shared<int>(1560),
            std::make_shared<int>(2011),
        };

        auto iterator = Algorithms::UninitializedMoveN(
            array,
            KITSUNE_ARRAY_SIZE(array),
            PointerContainer.GetBegin());

        EXPECT_EQ(iterator, PointerContainer.GetEnd());

        for (int index = 0; index < PointerContainer.Size(); ++index)
        {
            EXPECT_EQ(*PointerContainer[index], valueArray[index]);
            EXPECT_EQ(PointerContainer[index].use_count(), 1);
            EXPECT_EQ(array[index], nullptr);
        }

        std::destroy_n(PointerContainer.m_Array, PointerContainer.Size());
    }

    // Algorithms::UninitializedFill(Iter, Iter, const T&)
    TEST_F(UninitializedTest, UninitializedFill)
    {
        std::shared_ptr<int> pointer = std::make_shared<int>(5);
        auto endIterator = PointerContainer.GetBegin();

        Algorithms::Advance(endIterator, PointerContainer.Size());
        Algorithms::UninitializedFill(
            PointerContainer.GetBegin(),
            endIterator,
            pointer);

        for (int index = 0; index < PointerContainer.Size(); ++index)
        {
            EXPECT_EQ(PointerContainer[index], pointer);
            EXPECT_EQ(PointerContainer[index].use_count(), 6);
        }

        std::destroy_n(PointerContainer.m_Array, PointerContainer.Size());
    }

    // Algorithms::UninitializedFillN(Iter, Size, const T&)
    TEST_F(UninitializedTest, UninitializedFillN)
    {
        std::shared_ptr<int> pointer = std::make_shared<int>(5);
        auto iterator = Algorithms::UninitializedFillN(
            PointerContainer.GetBegin(),
            PointerContainer.Size(),
            pointer);

        EXPECT_EQ(iterator, PointerContainer.GetEnd());

        for (int index = 0; index < PointerContainer.Size(); ++index)
        {
            EXPECT_EQ(PointerContainer[index], pointer);
            EXPECT_EQ(PointerContainer[index].use_count(), 6);
        }

        std::destroy_n(PointerContainer.m_Array, PointerContainer.Size());
    }
}
