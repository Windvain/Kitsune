#include "Foundation/Containers/Array.h"
#include <gtest/gtest.h>

#include "TestContainer.h"
#include "Foundation/Algorithms/Find.h"
#include "Foundation/Concepts/Container.h"

using namespace Kitsune;
using namespace Kitsune::Testing;

namespace
{
    class NotingAllocator
    {
    public:
        NotingAllocator() = default;
        NotingAllocator(void** allocator, void** freed)
            : m_Allocated(allocator), m_Freed(freed)
        {
        }

        NotingAllocator(const NotingAllocator&) = default;
        NotingAllocator(NotingAllocator&& allocator)
            : m_Allocated(std::exchange(allocator.m_Allocated, nullptr)),
              m_Freed(std::exchange(allocator.m_Freed, nullptr))
        {
        }

        ~NotingAllocator() = default;

    public:
        void* Allocate(Usize size, Usize align = __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            void* ptr = Memory::Allocate(size, align);
            if (m_Allocated)
                *m_Allocated = ptr;

            return ptr;
        }

        void Free(void* ptr, Usize size)
        {
            if (m_Freed)
                *m_Freed = ptr;

            Memory::Free(ptr, size);
        }

    private:
        void** m_Allocated = nullptr;
        void** m_Freed = nullptr;
    };

    [[maybe_unused]]
    bool operator==(const NotingAllocator&, const NotingAllocator&)
    {
        return true;
    }

    class TestAllocator : public Kitsune::GlobalAllocator
    {
    public:
        TestAllocator() = default;
        explicit TestAllocator(int id)
            : Id(id)
        {
        }

        TestAllocator(TestAllocator&& allocator)
            : Id(std::exchange(allocator.Id, 0))
        {
        }

        TestAllocator& operator=(TestAllocator&& a)
        {
            Id = std::exchange(a.Id, 0);
            return *this;
        }

        ~TestAllocator() = default;

    public:
        TestAllocator(const TestAllocator&) = default;
        TestAllocator& operator=(const TestAllocator&) = default;

    public:
        bool operator==(const TestAllocator& allocator) const
        {
            return (Id == allocator.Id);
        }

    public:
        int Id = 0;
    };
}

static_assert(
    Container<Array<int>>,
    "Array does not satisfy the Container concept.");

TEST(ArrayTests, DefaultConstructor)
{
    Array<int> array;

    EXPECT_EQ(array.Size(), 0);
    EXPECT_EQ(array.Capacity(), 0);
    EXPECT_EQ(array.Data(), nullptr);
}

TEST(ArrayTests, AllocatorConstructor)
{
    TestAllocator allocator = TestAllocator(23);
    auto copied = Array<int, TestAllocator>(allocator);

    EXPECT_EQ(allocator.Id, 23);
    EXPECT_EQ(copied.Data(), nullptr);
    EXPECT_EQ(copied.GetAllocator().Id, 23);
    EXPECT_GE(copied.Capacity(), 0);
    EXPECT_EQ(copied.Size(), 0);
}

TEST(ArrayTests, CapacityConstructor)
{
    TestAllocator allocator = TestAllocator(23);
    auto array = Array<int, TestAllocator>(100, allocator);

    EXPECT_EQ(allocator.Id, 23);
    EXPECT_EQ(array.GetAllocator().Id, 23);
    EXPECT_GE(array.Capacity(), 100);
    EXPECT_EQ(array.Size(), 0);

    auto empty = Array<int, TestAllocator>(0, TestAllocator(12));
    EXPECT_EQ(empty.GetAllocator().Id, 12);
    EXPECT_EQ(empty.Capacity(), 0);
    EXPECT_EQ(empty.Size(), 0);
    EXPECT_EQ(empty.Data(), nullptr);
}

TEST(ArrayTests, FillConstructor)
{
    TestAllocator allocator(22);
    Array<int, TestAllocator> array(6, int(97), allocator);

    EXPECT_EQ(allocator.Id, 22);
    EXPECT_EQ(array.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 6);

    for (int element : array)
        EXPECT_EQ(element, 97);
}

TEST(ArrayTests, RangeConstructor)
{
    TestAllocator allocator(22);
    std::vector<int> vector = { 1, 2, 3, 4, 56 };

    ForwardTestContainer<int, 5> container({ 1, 2, 3, 4, 56 });
    Array<int, TestAllocator> array(
        container.GetBegin(), container.GetEnd(), allocator);

    EXPECT_EQ(allocator.Id, 22);
    EXPECT_EQ(array.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);
}

TEST(ArrayTests, InitializerListConstructor)
{
    TestAllocator allocator(22);

    std::vector<int> vector = { 1, 2, 3, 4, 56 };
    Array<int, TestAllocator> array({ 1, 2, 3, 4, 56 }, allocator);

    EXPECT_EQ(allocator.Id, 22);
    EXPECT_EQ(array.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);
}

TEST(ArrayTests, CopyConstructor)
{
    std::vector<int> vector = { 23, 2, 65, 12, 98 };

    Array<int, TestAllocator> array({ 23, 2, 65, 12, 98 }, TestAllocator(33));
    auto copy = array;

    EXPECT_EQ(array.GetAllocator().Id, 33);
    EXPECT_EQ(copy.GetAllocator().Id, 33);

    for (std::size_t i = 0; i < array.Size(); ++i)
    {
        EXPECT_EQ(array[i], vector[i]);
        EXPECT_EQ(copy[i], vector[i]);
    }
}

TEST(ArrayTests, MoveConstructor)
{
    Array<int, TestAllocator> array({ 23, 2, 65, 12, 98 }, TestAllocator(33));
    std::vector<int> vector = { 23, 2, 65, 12, 98 };

    auto movedArray = std::move(array);

    EXPECT_EQ(movedArray.Size(), 5);
    for (std::size_t i = 0; i < movedArray.Size(); ++i)
        EXPECT_EQ(movedArray[i], vector[i]);

    EXPECT_EQ(array.Size(), 0);

    EXPECT_EQ(array.GetAllocator().Id, 0);
    EXPECT_EQ(movedArray.GetAllocator().Id, 33);
}

TEST(ArrayTests, Destructor)
{
    int* allocated = (int*)32;
    int* freed = (int*)3;

    {
        auto array = Array<int, NotingAllocator>(
            { 3, 43, 451, 598, 1 },
            NotingAllocator((void**)&allocated, (void**)&freed));
    }

    EXPECT_EQ(allocated, freed);
}

TEST(ArrayTests, CopyAssign)
{
    Array<int, TestAllocator> array({ 1, 34, 65, 234, 123 }, TestAllocator(321));
    Array<int, TestAllocator> copiedArray({ 234, 54 }, TestAllocator(32));

    array = copiedArray;

    EXPECT_EQ(array.GetAllocator(), copiedArray.GetAllocator());
    EXPECT_EQ(array.Size(), copiedArray.Size());

    for (Usize i = 0; i < array.Size(); ++i)
        EXPECT_EQ(copiedArray[i], array[i]);
}

TEST(ArrayTests, MoveAssign)
{
    Array<int, TestAllocator> array({ 1, 2, 3, 52, 21 }, TestAllocator(32));

    TestAllocator allocator = TestAllocator(74);
    Array<int, TestAllocator> movedArray({ 1, 25, 124, 13 }, allocator);

    std::vector<int> vector = { 1, 25, 124, 13 };
    array = std::move(movedArray);

    EXPECT_EQ(array.GetAllocator(), allocator);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);
}

TEST(ArrayTests, InitializerListAssign)
{
    TestAllocator allocator = TestAllocator(42);

    std::vector<int> vector = { 1, 2, 3, 4, 4, 1, 2 };
    Array<int, TestAllocator> array({ 1, 2, 3, 52, 21 }, allocator);

    array = { 1, 2, 3, 4, 4, 1, 2 };

    EXPECT_EQ(array.GetAllocator(), allocator);
    EXPECT_EQ(array.Size(), 7);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    array = { /* ... */ };

    EXPECT_EQ(array.GetAllocator(), allocator);
    EXPECT_EQ(array.Size(), 0);
}

TEST(ArrayTests, Subscript)
{
    Array<int> array = { 1, 2, 3, 45, 21 };
    for (Index i = 0; i < 5; ++i)
        EXPECT_EQ(&array[i], &array.Data()[i]);

    EXPECT_THROW(array[5], OutOfRangeException);
}

TEST(ArrayTests, FrontBack)
{
    Array<int> array = { 1, 2, 43, 123, 451, 123 };
    Array<int> empty(53);

    EXPECT_EQ(&array.Front(), array.Data());
    EXPECT_EQ(&array.Back(), array.Data() + array.Size() - 1);

    EXPECT_THROW(KITSUNE_UNUSED(empty.Back()), OutOfRangeException);
    EXPECT_THROW(KITSUNE_UNUSED(empty.Front()), OutOfRangeException);
}

TEST(ArrayTests, IsEmpty)
{
    Array<int> array = { 1, 2, 323, 432, 123 };
    Array<int> empty(52);

    ASSERT_EQ(array.Size(), 5);
    ASSERT_EQ(empty.Size(), 0);

    EXPECT_FALSE(array.IsEmpty());
    EXPECT_TRUE(empty.IsEmpty());
}

TEST(ArrayTests, Iterators)
{
    Array<int> array = { 54, 12, 390, 16 };

    EXPECT_EQ(ToAddress(array.GetBegin()), array.Data());
    EXPECT_EQ(ToAddress(array.GetReverseBegin()), &array.Back());

    EXPECT_EQ(array.GetEnd(), array.GetBegin() + array.Size());
    EXPECT_EQ(array.GetReverseEnd(), array.GetReverseBegin() + array.Size());
}

TEST(ArrayTests, Reserve)
{
    Array<int> array = { 32, 5, 12, 54 };
    std::vector<int> vector = { 32, 5, 12, 54 };

    Usize prevCapacity = array.Capacity();
    array.Reserve(1);

    EXPECT_GE(array.Capacity(), prevCapacity);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    array.Reserve(4);

    EXPECT_GE(array.Capacity(), prevCapacity);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    array.Reserve(12);

    EXPECT_GE(array.Capacity(), 12);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);
}

TEST(ArrayTests, ShrinkToFit)
{
    Array<int> array = { 1, 2, 334, 532 };
    std::vector<int> vector = { 1, 2, 334, 532 };

    array.Reserve(100);

    ASSERT_GE(array.Capacity(), 100);
    array.ShrinkToFit();

    EXPECT_EQ(array.Size(), 4);
    EXPECT_EQ(array.Capacity(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);
}

TEST(ArrayTests, Resize)
{
    Array<int> array = { 938, 123, 12, 499, 72 };
    array.Resize(array.Size());     // No-op

    int expected1[5] = { 938, 123, 12, 499, 72 };
    EXPECT_EQ(array.Size(), 5);

    for (Index i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array.Data()[i], expected1[i]);

    array.Resize(3);    // Truncates array.

    int expected2[3] = { 938, 123, 12 };
    EXPECT_EQ(array.Size(), 3);

    for (Index i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array.Data()[i], expected2[i]);

    array.Resize(6);    // Extends array.

    int expected3[6] = { 938, 123, 12, 0, 0, 0 };
    EXPECT_EQ(array.Size(), 6);

    for (Index i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array.Data()[i], expected3[i]);

    array.Resize(8, int(9));        // Extends array with int(9).

    int expected4[8] = { 938, 123, 12, 0, 0, 0, 9, 9 };
    EXPECT_EQ(array.Size(), 8);

    for (Index i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array.Data()[i], expected4[i]);
}

TEST(ArrayTests, Clear)
{
    Array<int> array = { 32, 654, 123, 32, 983, 435 };
    array.Clear();

    EXPECT_EQ(array.Size(), 0);
}

TEST(ArrayTests, InsertCopy)
{
    Array<int> array = { 32, 54, 12, 9834, 9 };
    std::vector<int> vector = { 3, 32, 54, 12, 9834, 9 };

    int value1(3);
    auto iter = array.Insert(array.GetBegin(), value1);

    EXPECT_EQ(iter, array.GetBegin());

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    std::vector<int> vec2 = { 3, 32, 54, 4, 12, 9834, 9 };

    int value2(4);
    auto iter2 = array.Insert(array.GetBegin() + 3, value2);

    EXPECT_EQ(iter2, array.GetBegin() + 3);
    EXPECT_TRUE(array[3]);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    std::vector<int> vec3 = { 3, 32, 54, 4, 12, 9834, 9, 5 };

    int value3(5);
    auto iter3 = array.Insert(array.GetEnd(), value3);

    EXPECT_EQ(iter3, array.GetEnd() - 1);
    EXPECT_TRUE(array.Back());

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec3[i]);

    int value4(4);
    EXPECT_THROW(array.Insert(array.GetEnd() + 1, value4), OutOfRangeException);
}

TEST(ArrayTests, InsertMove)
{
    Array<int> array = { 32, 54, 12, 9834, 9 };
    std::vector<int> vector = { 3, 32, 54, 12, 9834, 9 };

    auto iter = array.Insert(array.GetBegin(), int(3));
    EXPECT_EQ(iter, array.GetBegin());

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    std::vector<int> vec2 = { 3, 32, 54, 4, 12, 9834, 9 };
    auto iter2 = array.Insert(array.GetBegin() + 3, int(4));

    EXPECT_EQ(iter2, array.GetBegin() + 3);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    std::vector<int> vec3 = { 3, 32, 54, 4, 12, 9834, 9, 5 };
    auto iter3 = array.Insert(array.GetEnd(), 5);

    EXPECT_EQ(iter3, array.GetEnd() - 1);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec3[i]);

    EXPECT_THROW(array.Insert(array.GetEnd() + 1, int(4)), OutOfRangeException);
}

TEST(ArrayTests, InsertFill)
{
    Array<int> array = { 54, 657, 123, 677 };
    std::vector<int> vector = { 4, 4, 4, 4, 4, 4, 4, 54, 657, 123, 677 };

    auto iter = array.Insert(array.GetBegin(), 7, int(4));
    EXPECT_EQ(iter, array.GetBegin());

    EXPECT_EQ(array.Size(), vector.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    std::vector<int> vec2 = { 4, 4, 4, 4, 4, 4, 4, 54, 657, 123, 677, 2, 2, 2 };
    auto iter2 = array.Insert(array.GetEnd(), 3, int(2));

    EXPECT_EQ(iter2, array.GetBegin() + 11);
    EXPECT_EQ(array.Size(), vec2.size());

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    std::vector<int> vec3 = { 4, 4, 4, 4, 4, 4, 4, 54, 0, 0, 657, 123, 677, 2, 2, 2 };
    auto iter3 = array.Insert(array.GetBegin() + 8, 2, int(0));

    EXPECT_EQ(iter3, array.GetBegin() + 8);
    EXPECT_EQ(array.Size(), vec3.size());

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec3[i]);

    auto iter4 = array.Insert(array.GetBegin() + 2, 0, int(342));
    EXPECT_EQ(iter4, array.GetBegin() + 2);

    EXPECT_EQ(array.Size(), vec3.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec3[i]);

    EXPECT_THROW(array.Insert(array.GetBegin() - 1, 2, int(5)), OutOfRangeException);
}

TEST(ArrayTests, InsertRange)
{
    Array<int> array;
    ForwardTestContainer<int, 3> range1({ 43, 6, 5 });

    auto iter = array.Insert(array.GetBegin(), range1.GetBegin(), range1.GetEnd());

    EXPECT_EQ(iter, &array.Front());
    EXPECT_EQ(array.Size(), 3);

    EXPECT_EQ(array[0], 43);
    EXPECT_EQ(array[1], 6);
    EXPECT_EQ(array[2], 5);

    ForwardTestContainer<int, 2> range2({ 3, 55 });
    auto iter2 = array.Insert(array.GetBegin() + 1, range2.GetBegin(), range2.GetEnd());

    std::vector<int> vec2 = { 43, 3, 55, 6, 5 };

    EXPECT_EQ(iter2, &array.Front() + 1);
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    auto iter3 = array.Insert(array.GetBegin(), (int*)nullptr, (int*)nullptr);
    EXPECT_EQ(iter3, array.GetBegin());
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    ForwardTestContainer<int, 2> range3({ 45, 65 });

    auto iter4 = array.Insert(array.GetEnd(), range3.GetBegin(), range3.GetEnd());
    std::vector<int> vec4 = { 43, 3, 55, 6, 5, 45, 65 };

    auto expectedIt4 = array.GetEnd();
    --expectedIt4; --expectedIt4;

    EXPECT_EQ(iter4, expectedIt4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec4[i]);

    ASSERT_NE(array.GetEnd(), nullptr);

    ForwardTestContainer<int, 2> range4({ 65, 9 });
    EXPECT_THROW(array.Insert(
        array.GetBegin() - 1,
        range4.GetBegin(), range4.GetEnd()), OutOfRangeException);
}

TEST(ArrayTests, InsertInitializerList)
{
    Array<int> array;
    auto iter = array.Insert(array.GetBegin(), { 43, 6, 5 });

    EXPECT_EQ(iter, &array.Front());
    EXPECT_EQ(array.Size(), 3);

    EXPECT_EQ(array[0], 43);
    EXPECT_EQ(array[1], 6);
    EXPECT_EQ(array[2], 5);

    auto iter2 = array.Insert(array.GetBegin() + 1, { 3, 55 });
    std::vector<int> vec2 = { 43, 3, 55, 6, 5 };

    EXPECT_EQ(iter2, &array.Front() + 1);
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    auto iter3 = array.Insert(array.GetBegin(), {  });
    EXPECT_EQ(iter3, array.GetBegin());
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    auto iter4 = array.Insert(array.GetEnd(), { 45, 65 });
    std::vector<int> vec4 = { 43, 3, 55, 6, 5, 45, 65 };

    auto expectedIt4 = array.GetEnd();
    --expectedIt4; --expectedIt4;

    EXPECT_EQ(iter4, expectedIt4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec4[i]);

    ASSERT_NE(array.GetEnd(), nullptr);
    EXPECT_THROW(array.Insert(array.GetBegin() - 1, { 65, 9 }), OutOfRangeException);
}

TEST(ArrayTests, Remove)
{
    Array<int> array = { 54, 657, 123, 677 };
    array.Remove(array.GetBegin() + 2);

    EXPECT_EQ(array.Size(), 3);

    EXPECT_EQ(array[0], 54);
    EXPECT_EQ(array[1], 657);
    EXPECT_EQ(array[2], 677);

    array.Clear();
    ASSERT_EQ(array.Size(), 0);

    EXPECT_THROW(array.Remove(array.GetEnd()), OutOfRangeException);
}

TEST(ArrayTests, RemoveRange)
{
    Array<int> array = { 54, 657, 123, 677, 65, 11, 540 };
    std::vector<int> vector = { 54, 657, 123, 540 };

    array.Remove(array.GetBegin() + 3, array.GetEnd() - 1);

    EXPECT_EQ(array.Size(), vector.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    array.Remove(array.GetBegin(), array.GetBegin());

    EXPECT_EQ(array.Size(), vector.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vector[i]);

    array.Remove(array.GetBegin(), array.GetEnd());
    EXPECT_EQ(array.Size(), 0);

    Array<int> array2 = { 54, 92, 172, 5 };
    EXPECT_THROW(array2.Remove(
        array2.GetBegin(), array2.GetEnd() + 1), OutOfRangeException);
}

TEST(ArrayTests, RemoveUnsorted)
{
    Array<int> array = { 54, 657, 123, 677 };
    std::vector<int> vector = { 657, 123, 677 };

    array.RemoveUnsorted(array.GetBegin());
    EXPECT_EQ(array.Size(), 3);

    for (Usize i = 0; i < vector.size(); ++i)
    {
        EXPECT_NE(Algorithms::Find(array.GetBegin(), array.GetEnd(), vector[i]), array.GetEnd());
    }

    array.Clear();
    ASSERT_EQ(array.Size(), 0);

    EXPECT_THROW(array.RemoveUnsorted(array.GetEnd()), OutOfRangeException);
}

TEST(ArrayTests, RemoveUnsortedRange)
{
    Array<int> array = { 54, 657, 123, 677, 65, 11, 540 };
    std::vector<int> vector = { 54, 657, 11, 540 };

    array.RemoveUnsorted(array.GetBegin() + 2, array.GetEnd() - 2);

    EXPECT_EQ(array.Size(), vector.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
    {
        EXPECT_NE(Algorithms::Find(array.GetBegin(), array.GetEnd(), vector[i]), array.GetEnd());
    }

    array.Remove(array.GetBegin(), array.GetBegin());

    EXPECT_EQ(array.Size(), vector.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
    {
        EXPECT_NE(Algorithms::Find(array.GetBegin(), array.GetEnd(), vector[i]), array.GetEnd());
    }

    array.Remove(array.GetBegin(), array.GetEnd());
    EXPECT_EQ(array.Size(), 0);

    Array<int> array2 = { 54, 92, 172, 5 };
    EXPECT_THROW(array2.RemoveUnsorted(
        array2.GetBegin(), array2.GetEnd() + 1), OutOfRangeException);
}

TEST(ArrayTests, PushBackCopy)
{
    Array<int> array = { 453, 431 };
    int x = 534;

    array.PushBack(x);

    EXPECT_EQ(array.Size(), 3);
    EXPECT_EQ(array[0], 453);
    EXPECT_EQ(array[1], 431);
    EXPECT_EQ(array[2], 534);
}

TEST(ArrayTests, PushBackMove)
{
    Array<int> array = { 453, 431 };
    array.PushBack(int(534));

    EXPECT_EQ(array.Size(), 3);
    EXPECT_EQ(array[0], 453);
    EXPECT_EQ(array[1], 431);
    EXPECT_EQ(array[2], 534);
}

TEST(ArrayTests, EmplaceBack)
{
    Array<int> array = { 453, 431 };
    int& back = array.EmplaceBack(534);

    EXPECT_EQ(&back, &array.Back());

    EXPECT_EQ(array.Size(), 3);
    EXPECT_EQ(array[0], 453);
    EXPECT_EQ(array[1], 431);
    EXPECT_EQ(array[2], 534);
}

TEST(ArrayTests, PopBack)
{
    Array<int> array = { 43, 675, 123 };
    array.PopBack();

    EXPECT_EQ(array.Size(), 2);
    EXPECT_EQ(array[0], 43);
    EXPECT_EQ(array[1], 675);

    array.PopBack();
    array.PopBack();

    EXPECT_EQ(array.Size(), 0);
    EXPECT_THROW(array.PopBack(), OutOfRangeException);
}

TEST(ArrayTests, RangedForLoop)
{
    Array<int> array = { 544, 123, 12, 7 };
    std::vector<int> vector = { 544, 123, 12, 7 };

    int index = 0;
    for (int element : array)
    {
        EXPECT_EQ(element, vector[index]);
        ++index;
    }
}

TEST(ArrayTests, Equal)
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

TEST(ArrayTests, Swap)
{
    Array<int> array1 = { 14, 23, 4343, 121 };
    Array<int> array2 = { 32, 54, 11, 43, 11 };

    array1.Swap(array2);

    EXPECT_EQ(array1.Size(), 5);
    EXPECT_EQ(array2.Size(), 4);

    EXPECT_EQ(array1[0], 32);
    EXPECT_EQ(array1[1], 54);
    EXPECT_EQ(array1[2], 11);
    EXPECT_EQ(array1[3], 43);
    EXPECT_EQ(array1[4], 11);

    EXPECT_EQ(array2[0], 14);
    EXPECT_EQ(array2[1], 23);
    EXPECT_EQ(array2[2], 4343);
    EXPECT_EQ(array2[3], 121);
}
