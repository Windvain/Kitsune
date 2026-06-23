#include <algorithm>
#include <gtest/gtest.h>

#include "TestContainer.h"
#include "StatefulAllocator.h"
#include "TrackingAllocator.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Concepts/Container.h"

namespace
{
    using namespace Kitsune;
    using Testing::ForwardTestContainer, Testing::TrackingAllocator,
          Testing::StatefulAllocator;

    static_assert(
        Container<Array<std::string>>,
        "The Array<T, Alloc> class does not satisfy the requirements for being a "
        "Container.");

    // Array<T, Alloc>::Array()
    TEST(ArrayTest, DefaultConstructor)
    {
        Array<int, TrackingAllocator> array;
        EXPECT_EQ(array.GetAllocator().AllocationCount(), 0);

        EXPECT_EQ(array.Size(), 0);
        EXPECT_EQ(array.Capacity(), 0);
        EXPECT_EQ(array.Data(), nullptr);
    }

    // Array<T, Alloc>::Array(const Alloc&)
    TEST(ArrayTest, AllocatorConstructor)
    {
        StatefulAllocator allocator(23);
        Array<int, StatefulAllocator> array(allocator);

        EXPECT_EQ(array.GetAllocator().GetId(), allocator.GetId());

        EXPECT_EQ(array.Size(), 0);
        EXPECT_EQ(array.Capacity(), 0);
        EXPECT_EQ(array.Data(), nullptr);
    }

    // Array<T, Alloc>::Array(Usize)
    TEST(ArrayTest, CapacityConstructor)
    {
        Array<int, TrackingAllocator> array(100);
        Usize capacity = array.GetAllocator().AllocationSize(array.Data());

        EXPECT_EQ(array.Size(), 0);
        EXPECT_EQ(array.GetAllocator().AllocationCount(), 1);

        EXPECT_EQ(array.Capacity(), capacity / sizeof(int));
        EXPECT_GE(capacity, 100 * sizeof(int));

        Array<int, StatefulAllocator> array2(100, StatefulAllocator(12));
        EXPECT_EQ(array2.Size(), 0);
        EXPECT_GE(array2.Capacity(), 100);
        EXPECT_EQ(array2.GetAllocator().GetId(), 12);

        Array<int, TrackingAllocator> empty(0);
        EXPECT_EQ(empty.Size(), 0);
        EXPECT_GE(empty.Capacity(), 0);
        EXPECT_EQ(empty.GetAllocator().AllocationCount(), 0);
    }

    // Array<T, Alloc>::Array(Usize, const T&)
    TEST(ArrayTest, FillConstructor)
    {
        Array<int, TrackingAllocator> array(20, int(97));
        Usize capacity = array.GetAllocator().AllocationSize(array.Data());

        EXPECT_EQ(array.Size(), 20);
        EXPECT_EQ(array.Capacity(), capacity / sizeof(int));
        EXPECT_GE(capacity, 20 * sizeof(int));

        for (int index = 0; index < 20; ++index)
            EXPECT_EQ(array.Data()[index], 97);

        Array<int, StatefulAllocator> array2(14, int(33), StatefulAllocator(4));
        EXPECT_EQ(array2.GetAllocator().GetId(), 4);

        EXPECT_EQ(array2.Size(), 14);
        EXPECT_GE(array2.Capacity(), 14);

        for (int index = 0; index < 14; ++index)
            EXPECT_EQ(array2.Data()[index], 33);
    }

    // Array<T, Alloc>::Array(Iter, Iter)
    TEST(ArrayTest, RangeConstructor)
    {
        ForwardTestContainer<int, 5> source = { 23, 4, 1, 343, 9 };

        Array<int, TrackingAllocator> array(source.GetBegin(), source.GetEnd());
        Usize capacity = array.GetAllocator().AllocationSize(array.Data());

        EXPECT_EQ(array.Size(), 5);
        EXPECT_EQ(array.Capacity(), capacity / sizeof(int));
        EXPECT_GE(capacity, source.Size() * sizeof(int));

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array.Data()[index], source[index]);

        Array<int, StatefulAllocator> array2(
            source.GetBegin(), source.GetEnd(),
            StatefulAllocator(7));

        EXPECT_GE(array2.GetAllocator().GetId(), 7);

        EXPECT_EQ(array2.Size(), 5);
        EXPECT_GE(array2.Capacity(), 5);

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array2.Data()[index], source[index]);
    }

    // Array<T, Alloc>::Array(std::initializer_list<T>)
    TEST(ArrayTest, InitializerListConstructor)
    {
        std::vector<int> source = { 23, 4, 1, 343, 9 };

        Array<int, TrackingAllocator> array = { 23, 4, 1, 343, 9 };
        Usize capacity = array.GetAllocator().AllocationSize(array.Data());

        EXPECT_EQ(array.Size(), 5);
        EXPECT_GE(array.Capacity(), capacity / sizeof(int));
        EXPECT_GE(capacity, source.size() * sizeof(int));

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array.Data()[index], source[index]);

        Array<int, StatefulAllocator> array2({ 23, 4, 1, 343, 9 }, StatefulAllocator(7));
        EXPECT_GE(array2.GetAllocator().GetId(), 7);

        EXPECT_EQ(array2.Size(), 5);
        EXPECT_GE(array2.Capacity(), 5);

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array2.Data()[index], source[index]);
    }

    // Array<T, Alloc>::Array(const Array&)
    TEST(ArrayTest, CopyConstructor)
    {
        Array<std::shared_ptr<int>, TrackingAllocator> array = {
            std::make_shared<int>(23),
            std::make_shared<int>(2),
            std::make_shared<int>(65),
            std::make_shared<int>(12),
            std::make_shared<int>(98)
        };

        Array<std::shared_ptr<int>, TrackingAllocator> copy = array;

        Usize capacity = array.GetAllocator().AllocationSize(array.Data());
        Usize copyCapacity = copy.GetAllocator().AllocationSize(copy.Data());

        EXPECT_GE(capacity, 5 * sizeof(std::shared_ptr<int>));
        EXPECT_GE(copyCapacity, 5 * sizeof(std::shared_ptr<int>));

        EXPECT_EQ(array.Capacity(), capacity / sizeof(std::shared_ptr<int>));
        EXPECT_EQ(copy.Capacity(), copyCapacity / sizeof(std::shared_ptr<int>));

        EXPECT_EQ(array.Size(), 5);
        EXPECT_EQ(copy.Size(), 5);

        std::vector<int> expected = { 23, 2, 65, 12, 98 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(*array.Data()[index], expected[index]);
            EXPECT_EQ(*copy.Data()[index], expected[index]);

            EXPECT_EQ(array.Data()[index].use_count(), 2);
            EXPECT_EQ(copy.Data()[index].use_count(), 2);
        }
    }

    // Array<T, Alloc>::Array(const Array&)
    TEST(ArrayTest, CopyConstructorCopiesAllocator)
    {
        Array<int, StatefulAllocator> array({ 23, 2, 65, 12, 98 }, StatefulAllocator(324));

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        Array<int, StatefulAllocator> copy = array;
        EXPECT_EQ(array.GetAllocator().GetId(), 324);
        EXPECT_EQ(array.GetAllocator(), copy.GetAllocator());
    }

    // Array<T, Alloc>::Array(Array&&)
    TEST(ArrayTest, MoveConstructor)
    {
        Array<std::shared_ptr<int>, TrackingAllocator> array = {
            std::make_shared<int>(39),
            std::make_shared<int>(12),
            std::make_shared<int>(45),
            std::make_shared<int>(44),
            std::make_shared<int>(94)
        };

        Array<std::shared_ptr<int>, TrackingAllocator> move = std::move(array);
        Usize capacity = move.GetAllocator().AllocationSize(move.Data());

        EXPECT_GE(capacity, 5 * sizeof(std::shared_ptr<int>));
        EXPECT_EQ(move.Capacity(), capacity / sizeof(std::shared_ptr<int>));
        EXPECT_EQ(array.Capacity(), 0);

        EXPECT_EQ(move.Size(), 5);
        EXPECT_EQ(array.Size(), 0);

        std::vector<int> expected = { 39, 12, 45, 44, 94 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(move.Data()[index].use_count(), 1);
            EXPECT_EQ(*move.Data()[index], expected[index]);
        }
    }

    // Array<T, Alloc>::Array(Array&&)
    TEST(ArrayTest, MoveConstructorMovesAllocator)
    {
        Array<int, StatefulAllocator> array({ 23, 2, 65, 12, 98 }, StatefulAllocator(324));
        Array<int, StatefulAllocator> move = std::move(array);

        EXPECT_EQ(array.GetAllocator().GetId(), 0);
        EXPECT_EQ(move.GetAllocator().GetId(), 324);
    }

    // Array<T, Alloc>::~Array()
    TEST(ArrayTest, Destructor)
    {
        // TODO: Find a way to test the destructor.
        EXPECT_TRUE(true);
    }

    // Array<T, Alloc>::operator=(const Array<T, Alloc>&)
    TEST(ArrayTest, CopyAssign)
    {
        Array<std::shared_ptr<int>, TrackingAllocator> array = {
            std::make_shared<int>(12),
            std::make_shared<int>(55),
            std::make_shared<int>(1),
            std::make_shared<int>(96),
            std::make_shared<int>(111)
        };

        Array<std::shared_ptr<int>, TrackingAllocator> copy = {
            std::make_shared<int>(34),
            std::make_shared<int>(65),
            std::make_shared<int>(3),
        };

        copy = array;

        Usize capacity = array.GetAllocator().AllocationSize(array.Data());
        Usize copyCapacity = copy.GetAllocator().AllocationSize(copy.Data());

        EXPECT_EQ(copy.Size(), 5);
        EXPECT_EQ(array.Size(), 5);

        EXPECT_GE(copyCapacity, 5 * sizeof(std::shared_ptr<int>));
        EXPECT_EQ(copy.Capacity(), copyCapacity / sizeof(std::shared_ptr<int>));

        EXPECT_GE(capacity, 5 * sizeof(std::shared_ptr<int>));
        EXPECT_EQ(array.Capacity(), capacity / sizeof(std::shared_ptr<int>));

        std::vector<int> expected = { 12, 55, 1, 96, 111 };
        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(*array.Data()[index], expected[index]);
            EXPECT_EQ(*copy.Data()[index], expected[index]);

            EXPECT_EQ(array.Data()[index], copy.Data()[index]);
            EXPECT_EQ(array.Data()[index].use_count(), 2);
            EXPECT_EQ(copy.Data()[index].use_count(), 2);
        }
    }

    // Array<T, Alloc>::operator=(const Array<T, Alloc>&)
    TEST(ArrayTest, CopyAssignCopiesAllocator)
    {
        Array<int, StatefulAllocator> array({ 1, 32, 561, 12 }, StatefulAllocator(34));
        Array<int, StatefulAllocator> copy({ 32948, 1283, 1282 }, StatefulAllocator(4341));

        copy = array;

        EXPECT_EQ(array.GetAllocator().GetId(), 34);
        EXPECT_EQ(copy.GetAllocator().GetId(), 34);
    }

    // Array<T, Alloc>::operator=(Array<T, Alloc>&&)
    TEST(ArrayTest, MoveAssign)
    {
        ForwardTestContainer<std::shared_ptr<int>, 5> source = {
            std::make_shared<int>(12),
            std::make_shared<int>(55),
            std::make_shared<int>(1),
            std::make_shared<int>(96),
            std::make_shared<int>(111)
        };

        Array<std::shared_ptr<int>, TrackingAllocator> array(
            source.GetBegin(), source.GetEnd());

        Array<std::shared_ptr<int>, TrackingAllocator> move = {
            std::make_shared<int>(34),
            std::make_shared<int>(65),
            std::make_shared<int>(3),
        };

        move = std::move(array);

        Usize capacity = move.GetAllocator().AllocationSize(move.Data());

        EXPECT_EQ(move.Size(), 5);
        EXPECT_EQ(array.Size(), 0);

        EXPECT_EQ(array.Capacity(), 0);
        EXPECT_GE(capacity, 5 * sizeof(std::shared_ptr<int>));
        EXPECT_EQ(move.Capacity(), capacity / sizeof(std::shared_ptr<int>));

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(move.Data()[index], source[index]);
    }

    // Array<T, Alloc>::operator=(Array<T, Alloc>&&)
    TEST(ArrayTest, MoveAssignMovesAllocator)
    {
        Array<int, StatefulAllocator> array({ 49, 132, 59, 555, 1 }, StatefulAllocator(442));
        Array<int, StatefulAllocator> move = std::move(array);

        EXPECT_EQ(move.GetAllocator().GetId(), 442);
        EXPECT_EQ(array.GetAllocator().GetId(), 0);
    }

    // Array<T, Alloc>::operator=(std::initializer_list<T>)
    TEST(ArrayTest, InitializerListAssign)
    {
        Array<int, TrackingAllocator> array = { 34, 65, 3 };
        array = { 421, 4556, 3291, 9451 };

        Usize copyCapacity = array.GetAllocator().AllocationSize(array.Data());

        EXPECT_EQ(array.Size(), 4);
        EXPECT_GE(copyCapacity, 4 * sizeof(int));
        EXPECT_EQ(array.Capacity(), copyCapacity / sizeof(int));

        std::vector<int> expected = { 421, 4556, 3291, 9451 };
        for (int index = 0; index < 4; ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::operator[](Index)
    TEST(ArrayTest, Subscript)
    {
        Array<int> array = { 1, 2, 3, 45, 21 };
        const Array<int> constArray = { 1, 2, 3, 45, 21 };

        for (int index = 0; index < 5; ++index)
        {
            EXPECT_EQ(&array[index], &array.Data()[index]);
            EXPECT_EQ(&constArray[index], &constArray.Data()[index]);
        }

        EXPECT_THROW(array[5], OutOfRangeException);
        EXPECT_THROW(constArray[5], OutOfRangeException);
    }

    // Array<T, Alloc>::Front()
    // Array<T, Alloc>::Back()
    TEST(ArrayTest, FrontBack)
    {
        Array<int> array = { 1, 2, 43, 123, 451, 123 };
        const Array<int> constArray = { 1, 2, 43, 123, 451, 123 };

        Array<int> empty(53);
        const Array<int> constEmpty(53);

        EXPECT_EQ(&array.Front(), array.Data());
        EXPECT_EQ(&array.Back(), array.Data() + array.Size() - 1);

        EXPECT_EQ(&constArray.Front(), constArray.Data());
        EXPECT_EQ(&constArray.Back(), constArray.Data() + constArray.Size() - 1);

        EXPECT_THROW(KITSUNE_UNUSED(empty.Back()), OutOfRangeException);
        EXPECT_THROW(KITSUNE_UNUSED(empty.Front()), OutOfRangeException);

        EXPECT_THROW(KITSUNE_UNUSED(constEmpty.Back()), OutOfRangeException);
        EXPECT_THROW(KITSUNE_UNUSED(constEmpty.Front()), OutOfRangeException);
    }

    /* Tests expect Array<T, Alloc>::Data(), Array<T, Alloc>::GetAllocator(),
     * Array<T, Alloc>::Size(), and Array<T, Alloc>::Capacity() to work properly.
     */

    // Array<T, Alloc>::IsEmpty()
    TEST(ArrayTest, IsEmpty)
    {
        Array<int> array = { 843, 12, 11 };
        Array<int> empty(12);

        ASSERT_EQ(array.Size(), 3);
        ASSERT_EQ(empty.Size(), 0);

        EXPECT_FALSE(array.IsEmpty());
        EXPECT_TRUE(empty.IsEmpty());
    }

    // Array<T, Alloc>::GetBegin()
    // Array<T, Alloc>::GetEnd()
    // Array<T, Alloc>::GetReverseBegin()
    // Array<T, Alloc>::GetReverseEnd()
    TEST(ArrayTest, Iterators)
    {
        Array<int> array = { 54, 12, 390, 16 };
        const Array<int> constArray = { 54, 12, 390, 16 };

        EXPECT_EQ(ToAddress(array.GetBegin()), array.Data());
        EXPECT_EQ(ToAddress(array.GetReverseBegin()), &array.Back());
        EXPECT_EQ(ToAddress(constArray.GetBegin()), constArray.Data());
        EXPECT_EQ(ToAddress(constArray.GetReverseBegin()), &constArray.Back());

        EXPECT_EQ(array.GetEnd(), array.GetBegin() + array.Size());
        EXPECT_EQ(array.GetReverseEnd(), array.GetReverseBegin() + array.Size());

        EXPECT_EQ(constArray.GetEnd(), constArray.GetBegin() + constArray.Size());
        EXPECT_EQ(
            constArray.GetReverseEnd(),
            constArray.GetReverseBegin() + constArray.Size());
    }

    // Array<T, Alloc>::Resize(Usize)
    TEST(ArrayTest, Reserve)
    {
        Array<int> array = { 43, 565, 12, 54, 1 };

        ASSERT_LT(array.Capacity(), 100);
        array.Reserve(100);

        EXPECT_EQ(array.Size(), 5);
        EXPECT_GE(array.Capacity(), 100);

        std::vector<int> expected = { 43, 565, 12, 54, 1 };
        for (int index = 0; index < expected.size(); ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::Reserve(Usize)
    TEST(ArrayTest, ReserveLessThanCapacity)
    {
        Array<int> array = { 43, 565, 12, 54, 1, 45, 12 };
        Usize capacity = array.Capacity();

        // Reserve less than the capacity.
        ASSERT_GE(array.Capacity(), 7);
        array.Reserve(4);

        EXPECT_EQ(array.Size(), 7);
        EXPECT_GE(array.Capacity(), capacity);

        std::vector<int> expected = { 43, 565, 12, 54, 1, 45, 12 };
        for (int index = 0; index < expected.size(); ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);

        // Reserve exactly the capacity.
        array.Reserve(capacity);

        EXPECT_EQ(array.Size(), 7);
        EXPECT_GE(array.Capacity(), capacity);

        for (int index = 0; index < expected.size(); ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::ShrinkToFit()
    TEST(ArrayTest, ShrinkToFit)
    {
        Array<int> array = { 123, 56, 12, 7 };
        if (array.Capacity() == array.Size())
            array.Reserve(9);

        array.ShrinkToFit();
        EXPECT_EQ(array.Size(), 4);
        EXPECT_EQ(array.Capacity(), 4);

        std::vector<int> expected = { 123, 56, 12, 7 };
        for (int index = 0; index < 4; ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    /* Array<T, Alloc>::Resize(Usize count) is an alias for Resize(count, T()). */

    // Array<T, Alloc>::Resize(Usize)
    TEST(ArrayTest, ResizeNoOp)
    {
        Array<int> array = { 45, 1238, 1238, 945, 1 };
        array.Resize(array.Size());

        EXPECT_EQ(array.Size(), 5);
        EXPECT_GE(array.Capacity(), array.Size());

        std::vector<int> expected = { 45, 1238, 1238, 945, 1 };
        for (int index = 0; index < expected.size(); ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::Resize(Usize)
    TEST(ArrayTest, ResizeTruncation)
    {
        Array<int> array = { 45, 1238, 1238, 945, 1 };
        array.Resize(3);

        EXPECT_EQ(array.Size(), 3);
        EXPECT_GE(array.Capacity(), 3);

        std::vector<int> expected = { 45, 1238, 1238 };
        for (int index = 0; index < expected.size(); ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::Resize(Usize, T)
    TEST(ArrayTest, ResizeFill)
    {
        Array<int> array = { 45, 1238, 1238, 945, 1 };
        array.Resize(7, int(431));

        EXPECT_EQ(array.Size(), 7);
        EXPECT_GE(array.Capacity(), 7);

        std::vector<int> expected = { 45, 1238, 1238, 945, 1, 431, 431 };
        for (int index = 0; index < expected.size(); ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::Swap(Array<T, Alloc>&)
    TEST(ArrayTest, Swap)
    {
        Array<int, StatefulAllocator> array({ 23, 5, 1, 3, 65 }, StatefulAllocator(21));
        Array<int, StatefulAllocator> array2({ 22, 12, 23, 5, 1, 3, 65 }, StatefulAllocator(11));

        array.Swap(array2);

        EXPECT_EQ(array.GetAllocator().GetId(), 11);
        EXPECT_EQ(array2.GetAllocator().GetId(), 21);

        EXPECT_EQ(array.Size(), 7);
        EXPECT_EQ(array2.Size(), 5);

        EXPECT_GE(array.Capacity(), 7);
        EXPECT_GE(array2.Capacity(), 5);

        std::vector<int> expected = { 22, 12, 23, 5, 1, 3, 65 };
        std::vector<int> expected2 = { 23, 5, 1, 3, 65 };

        for (int index = 0; index < 7; ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array2.Data()[index], expected2[index]);
    }

    // Array<T, Alloc>::Clear()
    TEST(ArrayTest, Clear)
    {
        Array<int> array = { 32, 654, 123, 32, 983, 435 };
        array.Clear();

        EXPECT_EQ(array.Size(), 0);
    }

    // Array<T, Alloc>::Reset()
    TEST(ArrayTest, Reset)
    {
        Array<int> array = { 32, 654, 123, 32, 983, 435 };
        array.Reset();

        EXPECT_EQ(array.Size(), 0);
        EXPECT_EQ(array.Capacity(), 0);

        EXPECT_EQ(array.Data(), nullptr);
    }

    // Array<T, Alloc>::Assign(Iter, Iter)
    TEST(ArrayTest, AssignRange)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        const ForwardTestContainer<std::shared_ptr<int>, 7> container = {
            std::make_shared<int>(1),
            std::make_shared<int>(2),
            std::make_shared<int>(4),
            std::make_shared<int>(3),
            std::make_shared<int>(67),
            std::make_shared<int>(1),
            std::make_shared<int>(1)
        };

        array.Assign(container.GetBegin(), container.GetEnd());

        EXPECT_EQ(array.Size(), 7);
        EXPECT_GE(array.Capacity(), 7);

        for (int index = 0; index < 7; ++index)
        {
            EXPECT_EQ(array.Data()[index].use_count(), 2);
            EXPECT_EQ(array.Data()[index], container[index]);
        }
    }

    /* Array<T, Alloc>::Assign(std::initializer_list<T>) is tested by
     * operator=(std::initializer_list<T>). Skipped.
     */

    // Array<T, Alloc>::Assign(Usize, const T&)
    TEST(ArrayTest, AssignFill)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        std::shared_ptr<int> pointer = std::make_shared<int>(343);
        array.Assign(10, pointer);

        EXPECT_EQ(array.Size(), 10);
        EXPECT_GE(array.Capacity(), 10);

        for (int index = 0; index < 10; ++index)
        {
            EXPECT_EQ(array.Data()[index].use_count(), 11);
            EXPECT_EQ(array.Data()[index], pointer);
        }
    }

    /* Array<T, Alloc>::Insert(Iter, const T&) and Array<T, Alloc>::Insert(Iter, T&&) is
     * implemented with calls to Emplace(Iter, Args&&...). Skipped.
     */

    // Array<T, Alloc>::Insert(Iter, Usize, const T&)
    TEST(ArrayTest, InsertFill)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        std::shared_ptr<int> pointer = std::make_shared<int>(343);
        auto iterator = array.Insert(array.GetBegin() + 2, 10, pointer);

        EXPECT_EQ(iterator, array.GetBegin() + 2);

        EXPECT_EQ(array.Size(), 14);
        EXPECT_GE(array.Capacity(), 14);

        std::vector<int> expected = { 493, 12, 343, 343, 343, 343, 343,
                                      343, 343, 343, 343, 343, 54, 931 };

        for (int index = 0; index < 14; ++index)
            EXPECT_EQ(*array.Data()[index], expected[index]);

        for (int index = 2; index < 12; ++index)
        {
            EXPECT_EQ(array.Data()[index], pointer);
            EXPECT_EQ(array.Data()[index].use_count(), 11);
        }

        EXPECT_THROW(
            KITSUNE_UNUSED(array.Insert(array.GetBegin() - 1, 4, pointer)),
            OutOfRangeException);

        EXPECT_THROW(
            KITSUNE_UNUSED(array.Insert(array.GetEnd() + 1, 4, pointer)),
            OutOfRangeException);
    }

    // Array<T, Alloc>::Insert(Iter[== GetEnd()], Usize, const T&)
    TEST(ArrayTest, InsertFillEnd)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        std::shared_ptr<int> pointer = std::make_shared<int>(343);
        auto iterator = array.Insert(array.GetEnd(), 10, pointer);

        EXPECT_EQ(iterator, array.GetBegin() + 4);

        EXPECT_EQ(array.Size(), 14);
        EXPECT_GE(array.Capacity(), 14);

        std::vector<int> expected = { 493, 12, 54, 931, 343, 343, 343, 343, 343,
                                      343, 343, 343, 343, 343 };

        for (int index = 0; index < 14; ++index)
            EXPECT_EQ(*array.Data()[index], expected[index]);

        for (int index = 4; index < 14; ++index)
        {
            EXPECT_EQ(array.Data()[index], pointer);
            EXPECT_EQ(array.Data()[index].use_count(), 11);
         }
    }

    // Array<T, Alloc>::Insert(Iter, InputIter, InputIter)
    TEST(ArrayTest, InsertRange)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        ForwardTestContainer<std::shared_ptr<int>, 4> container = {
            std::make_shared<int>(343),
            std::make_shared<int>(124),
            std::make_shared<int>(973),
            std::make_shared<int>(120),
        };

        auto iterator = array.Insert(
            array.GetBegin() + 1,
            container.GetBegin(), container.GetEnd());

        EXPECT_EQ(iterator, array.GetBegin() + 1);

        EXPECT_EQ(array.Size(), 8);
        EXPECT_GE(array.Capacity(), 8);

        std::vector<int> expected = { 493, 343, 124, 973, 120, 12, 54, 931 };
        for (int index = 0; index < 8; ++index)
            EXPECT_EQ(*array.Data()[index], expected[index]);

        for (int index = 1; index < 5; ++index)
        {
            EXPECT_EQ(array.Data()[index], container[index - 1]);
            EXPECT_EQ(array.Data()[index].use_count(), 2);
        }

        EXPECT_THROW(
            KITSUNE_UNUSED(array.Insert(
                array.GetBegin() - 1,
                container.GetBegin(), container.GetEnd())),
            OutOfRangeException);

        EXPECT_THROW(
            KITSUNE_UNUSED(array.Insert(
                array.GetEnd() + 1,
                container.GetBegin(), container.GetEnd())),
            OutOfRangeException);
    }

    // Array<T, Alloc>::Insert(Iter, InputIter, InputIter)
    TEST(ArrayTest, InsertRangeEnd)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        ForwardTestContainer<std::shared_ptr<int>, 4> container = {
            std::make_shared<int>(343),
            std::make_shared<int>(124),
            std::make_shared<int>(973),
            std::make_shared<int>(120),
        };

        auto iterator = array.Insert(
            array.GetEnd(),
            container.GetBegin(), container.GetEnd());

        EXPECT_EQ(iterator, array.GetBegin() + 4);

        EXPECT_EQ(array.Size(), 8);
        EXPECT_GE(array.Capacity(), 8);

        std::vector<int> expected = { 493, 12, 54, 931, 343, 124, 973, 120 };
        for (int index = 0; index < 8; ++index)
            EXPECT_EQ(*array.Data()[index], expected[index]);

        for (int index = 4; index < 8; ++index)
        {
            EXPECT_EQ(array.Data()[index], container[index - 4]);
            EXPECT_EQ(array.Data()[index].use_count(), 2);
        }
    }

    // Array<T, Alloc>::Insert(Iter, std::initializer_list<T>)
    TEST(ArrayTest, InsertInitializerList)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        auto iterator = array.Insert(
            array.GetBegin() + 1,
            {
                std::make_shared<int>(343),
                std::make_shared<int>(124),
                std::make_shared<int>(973),
                std::make_shared<int>(120)
            });

        EXPECT_EQ(iterator, array.GetBegin() + 1);

        EXPECT_EQ(array.Size(), 8);
        EXPECT_GE(array.Capacity(), 8);

        std::vector<int> expected = { 493, 343, 124, 973, 120, 12, 54, 931 };
        for (int index = 0; index < 8; ++index)
            EXPECT_EQ(*array.Data()[index], expected[index]);


        EXPECT_THROW(
            KITSUNE_UNUSED(array.Insert(
                array.GetBegin() - 1,
                { std::shared_ptr<int>() })),
            OutOfRangeException);

        EXPECT_THROW(
            KITSUNE_UNUSED(array.Insert(
                array.GetEnd() + 1,
                { std::shared_ptr<int>() })),
            OutOfRangeException);
    }

    // Array<T, Alloc>::Insert(Iter, std::initializer_list<T>)
    TEST(ArrayTest, InsertInitializerListEnd)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(493),
            std::make_shared<int>(12),
            std::make_shared<int>(54),
            std::make_shared<int>(931)
        };

        auto iterator = array.Insert(
            array.GetEnd(),
            {
                std::make_shared<int>(343),
                std::make_shared<int>(124),
                std::make_shared<int>(973),
                std::make_shared<int>(120),
            });

        EXPECT_EQ(iterator, array.GetBegin() + 4);

        EXPECT_EQ(array.Size(), 8);
        EXPECT_GE(array.Capacity(), 8);

        std::vector<int> expected = { 493, 12, 54, 931, 343, 124, 973, 120 };
        for (int index = 0; index < 8; ++index)
            EXPECT_EQ(*array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::Emplace(Iter, Args&&...)
    TEST(ArrayTest, Emplace)
    {
        Array<std::string> array = {
            "ipsum ",
            "dolor ",
            "sit ",
            "amet",
        };

        std::vector<char> container = { 'L', 'o', 'r', 'e', 'm', ' ' };
        auto iterator = array.Emplace(array.GetBegin(), container.begin(), container.end());

        EXPECT_EQ(iterator, array.GetBegin());

        EXPECT_EQ(array.Size(), 5);
        EXPECT_GE(array.Capacity(), 5);

        std::vector<std::string> expected = {
            "Lorem ",
            "ipsum ",
            "dolor ",
            "sit ",
            "amet"
        };

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);

        EXPECT_THROW(
            KITSUNE_UNUSED(array.Emplace(
                array.GetBegin() - 1,
                container.begin(), container.end())),
            OutOfRangeException);

        EXPECT_THROW(
            KITSUNE_UNUSED(array.Emplace(
                array.GetEnd() + 1,
                container.begin(), container.end())),
            OutOfRangeException);
    }

    // Array<T, Alloc>::Remove(Iter)
    TEST(ArrayTest, Remove)
    {
        Array<int> array = { 12, 4, 65, 1, 11, 85, 33 };
        array.Remove(array.GetBegin() + 3);

        EXPECT_EQ(array.Size(), 6);
        EXPECT_GE(array.Capacity(), 6);

        std::vector<int> expected = { 12, 4, 65, 11, 85, 33 };
        for (int index = 0; index < 6; ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);

        EXPECT_THROW(
            array.Remove(array.GetBegin() - 1),
            OutOfRangeException);

        EXPECT_THROW(
            array.Remove(array.GetEnd()),
            OutOfRangeException);
    }

    // Array<T, Alloc>::Remove(Iter, Iter)
    TEST(ArrayTest, RemoveRange)
    {
        Array<int> array = { 12, 4, 65, 1, 11, 85, 33 };
        array.Remove(array.GetBegin() + 3, array.GetEnd() - 2);

        EXPECT_EQ(array.Size(), 5);
        EXPECT_GE(array.Capacity(), 5);

        std::vector<int> expected = { 12, 4, 65, 85, 33 };
        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);

        // One iterator inside, one iterator outside.
        EXPECT_THROW(
            array.Remove(array.GetBegin() - 1, array.GetBegin()),
            OutOfRangeException);

        EXPECT_THROW(
            array.Remove(array.GetEnd() - 1, array.GetEnd() + 1),
            OutOfRangeException);

        // Both iterators outside.
        EXPECT_THROW(
            array.Remove(array.GetBegin() - 2, array.GetBegin() - 1),
            OutOfRangeException);

        EXPECT_THROW(
            array.Remove(array.GetEnd(), array.GetEnd() + 1),
            OutOfRangeException);

        // One iterator inside, one iterator outside, but flipped.
        EXPECT_THROW(
            array.Remove(array.GetBegin(), array.GetBegin() - 1),
            OutOfRangeException);

        EXPECT_THROW(
            array.Remove(array.GetEnd() + 1, array.GetEnd() - 1),
            OutOfRangeException);

        // Both iterators outside, but flipped.
        EXPECT_THROW(
            array.Remove(array.GetBegin() - 1, array.GetBegin() - 2),
            OutOfRangeException);

        EXPECT_THROW(
            array.Remove(array.GetEnd() + 1, array.GetEnd()),
            OutOfRangeException);

        // Flipped iterators, but in range.
        EXPECT_THROW(
            array.Remove(array.GetEnd(), array.GetBegin()),
            OutOfRangeException);
    }

    // Array<T, Alloc>::RemoveUnsorted(Iter)
    TEST(ArrayTest, RemoveUnsorted)
    {
        Array<int> array = { 12, 4, 65, 1, 11, 85, 33 };
        array.RemoveUnsorted(array.GetBegin() + 3);

        EXPECT_EQ(array.Size(), 6);
        EXPECT_GE(array.Capacity(), 6);

        std::vector<int> expected = { 12, 4, 65, 11, 85, 33 };
        for (int index = 0; index < 6; ++index)
            EXPECT_EQ(std::count(array.Data(), array.Data() + 6, expected[index]), 1);

        EXPECT_THROW(
            array.RemoveUnsorted(array.GetBegin() - 1),
            OutOfRangeException);

        EXPECT_THROW(
            array.RemoveUnsorted(array.GetEnd()),
            OutOfRangeException);
    }

    // Array<T, Alloc>::RemoveUnsorted(Iter, Iter)
    TEST(ArrayTest, RemoveUnsortedRange)
    {
        Array<int> array = { 12, 4, 65, 1, 11, 85, 33 };
        array.RemoveUnsorted(array.GetBegin() + 3, array.GetEnd() - 2);

        EXPECT_EQ(array.Size(), 5);
        EXPECT_GE(array.Capacity(), 5);

        std::vector<int> expected = { 12, 4, 65, 85, 33 };
        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(std::count(array.Data(), array.Data() + 5, expected[index]), 1);

        // One iterator inside, one iterator outside.
        EXPECT_THROW(
            array.RemoveUnsorted(array.GetBegin() - 1, array.GetBegin()),
            OutOfRangeException);

        EXPECT_THROW(
            array.RemoveUnsorted(array.GetEnd() - 1, array.GetEnd() + 1),
            OutOfRangeException);

        // Both iterators outside.
        EXPECT_THROW(
            array.RemoveUnsorted(array.GetBegin() - 2, array.GetBegin() - 1),
            OutOfRangeException);

        EXPECT_THROW(
            array.RemoveUnsorted(array.GetEnd(), array.GetEnd() + 1),
            OutOfRangeException);

        // One iterator inside, one iterator outside, but flipped.
        EXPECT_THROW(
            array.RemoveUnsorted(array.GetBegin(), array.GetBegin() - 1),
            OutOfRangeException);

        EXPECT_THROW(
            array.RemoveUnsorted(array.GetEnd() + 1, array.GetEnd() - 1),
            OutOfRangeException);

        // Both iterators outside, but flipped.
        EXPECT_THROW(
            array.RemoveUnsorted(array.GetBegin() - 1, array.GetBegin() - 2),
            OutOfRangeException);

        EXPECT_THROW(
            array.RemoveUnsorted(array.GetEnd() + 1, array.GetEnd()),
            OutOfRangeException);

        // Flipped iterators, but in range.
        EXPECT_THROW(
            array.RemoveUnsorted(array.GetEnd(), array.GetBegin()),
            OutOfRangeException);
    }

    // Array<T, Alloc>::PushBack(const T&)
    TEST(ArrayTest, PushBackCopy)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(543),
            std::make_shared<int>(2),
            std::make_shared<int>(12),
            std::make_shared<int>(6),
            std::make_shared<int>(8),
            std::make_shared<int>(123)
        };

        std::shared_ptr<int> pointer = std::make_shared<int>(234);
        array.PushBack(pointer);

        EXPECT_EQ(array.Size(), 7);
        EXPECT_GE(array.Capacity(), 7);

        std::vector<int> expected = { 543, 2, 12, 6, 8, 123, 234 };
        for (int index = 0; index < 6; ++index)
            EXPECT_EQ(*array[index], expected[index]);

        EXPECT_EQ(array.Data()[6], pointer);
    }

    // Array<T, Alloc>::PushBack(T&&)
    TEST(ArrayTest, PushBackMove)
    {
        Array<std::shared_ptr<int>> array = {
            std::make_shared<int>(543),
            std::make_shared<int>(2),
            std::make_shared<int>(12),
            std::make_shared<int>(6),
            std::make_shared<int>(8),
            std::make_shared<int>(123)
        };

        array.PushBack(std::make_shared<int>(234));

        EXPECT_EQ(array.Size(), 7);
        EXPECT_GE(array.Capacity(), 7);

        std::vector<int> expected = { 543, 2, 12, 6, 8, 123, 234 };
        for (int index = 0; index < 6; ++index)
            EXPECT_EQ(*array[index], expected[index]);

        EXPECT_EQ(array.Data()[6].use_count(), 1);
    }

    // Array<T, Alloc>::EmplaceBack(Args&&...)
    TEST(ArrayTest, EmplaceBack)
    {
        Array<std::string> array = {
            "Lorem ",
            "ipsum ",
            "dolor ",
            "sit ",
        };

        std::vector<char> amet = { 'a', 'm', 'e', 't' };
        array.EmplaceBack(amet.begin(), amet.end());

        EXPECT_EQ(array.Size(), 5);
        EXPECT_GE(array.Capacity(), 5);

        std::vector<std::string> expected = {
            "Lorem ",
            "ipsum ",
            "dolor ",
            "sit ",
            "amet"
        };

        for (int index = 0; index < 5; ++index)
            EXPECT_EQ(array[index], expected[index]);
    }

    // Array<T, Alloc>::PopBack()
    TEST(ArrayTest, PopBack)
    {
        Array<int> array = { 23, 324, 7, 1, 5, 9, 1 };
        array.PopBack();

        EXPECT_EQ(array.Size(), 6);
        EXPECT_GE(array.Capacity(), 6);

        std::vector<int> expected = { 23, 324, 7, 1, 5, 9 };
        for (int index = 0; index < 6; ++index)
            EXPECT_EQ(array.Data()[index], expected[index]);
    }

    // Array<T, Alloc>::begin()
    // Array<T, Alloc>::end()
    TEST(ArrayTest, RangedForLoop)
    {
        Array<int> array = { 544, 123, 12, 7 };
        int index = 0;

        for (const int& element : array)
        {
            EXPECT_EQ(&element, array.Data() + index);
            ++index;
        }
    }

    // operator==(const Array<T, Alloc>&, const Array<T, Alloc>&)
    TEST(ArrayTest, Equal)
    {
        Array<int> array = { 12, 23, 23, 54, 64 };
        Array<int> array2 = { 12, 23, 23, 54, 64 };

        Array<int> diffContents = { 12, 22, 23, 54, 64 };
        Array<int> diffSize = { 12, 23, 23, 54, 64, 72 };

        EXPECT_TRUE(array == array2);
        EXPECT_FALSE(array == diffSize);
        EXPECT_FALSE(array == diffContents);

        EXPECT_FALSE(array != array2);
        EXPECT_TRUE(array != diffSize);
        EXPECT_TRUE(array != diffContents);
    }
}
