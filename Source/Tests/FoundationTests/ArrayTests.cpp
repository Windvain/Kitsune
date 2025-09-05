#include <gtest/gtest.h>
#include "TestContainer2.h"

#include "Foundation/Containers/Array.h"
#include "Foundation/Concepts/Container.h"

namespace
{
    class A
    {
    public:
        A() = default;
        A(void** alloc, void** freed)
            : m_Allocated(alloc), m_Freed(freed)
        {
        }

        A(const A&) = default;
        A(A&& alloc)
            : m_Allocated(std::exchange(alloc.m_Allocated, nullptr)),
              m_Freed(std::exchange(alloc.m_Freed, nullptr))
        {
            alloc.Moved = true;
        }

        ~A() = default;

    public:
        void* Allocate(Kitsune::Usize size,
                       Kitsune::Usize align = __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            void* ptr = Kitsune::Memory::Allocate(size, align);
            if (m_Allocated)
                *m_Allocated = ptr;

            return ptr;
        }

        void Free(void* ptr)
        {
            if (m_Freed)
                *m_Freed = ptr;

            Kitsune::Memory::Free(ptr);
        }

    public:
        bool Moved = false;

    private:
        void** m_Allocated = nullptr;
        void** m_Freed = nullptr;
    };

    [[maybe_unused]]
    bool operator==(const A& /* a1 */, const A& /* a2 */) { return true; }

    class TestAllocator : public Kitsune::GlobalAllocator
    {
    public:
        TestAllocator() = default;
        explicit TestAllocator(int id) : Id(id) { /* ... */ }

        TestAllocator(const TestAllocator&) = default;
        TestAllocator(TestAllocator&& a)
        {
            Id = std::exchange(a.Id, 0);
        }

        ~TestAllocator() = default;

    public:
        TestAllocator& operator=(const TestAllocator&) = default;
        TestAllocator& operator=(TestAllocator&& a) { Id = std::exchange(a.Id, 0); return *this; }

    public:
        bool operator==(const TestAllocator& alloc) const { return (Id == alloc.Id); }

    public:
        int Id = 0;
    };

    class O
    {
    public:
        O() = default;
        O(int id) : Id(id) { /* ... */ }

        O(const O&) = default;
        O(O&& obj) { Id = std::exchange(obj.Id, 0); }
        ~O() = default;

    public:
        O& operator=(const O&) = default;
        O& operator=(O&& obj) { Id = std::exchange(obj.Id, 0); return *this; }

    public:
        int Id = 0;
    };

    class B
    {
    public:
        B(int id) : Id(id) { /* ... */ }

        B(const B& b)
            : Id(b.Id), Copied(true)
        {
        }

        B(B&& b)
            : Id(std::exchange(b.Id, 0)),
              Moved(true)
        {
        }

    public:
        int Id = 0;

        bool Moved = false;
        bool Copied = false;
    };
}

using namespace Kitsune;
using namespace Testing;

static_assert(Container<Array<int>>, "Array does not satisfy the Container concept.");

TEST(ArrayTests, DefaultConstructor)
{
    Array<int> array;

    EXPECT_EQ(array.Size(), 0);
    EXPECT_EQ(array.Capacity(), 0);
    EXPECT_EQ(array.Data(), nullptr);
}

TEST(ArrayTests, AllocatorConstructor)
{
    TestAllocator alloc = TestAllocator(23);
    auto copied = Array<int, TestAllocator>(alloc);

    EXPECT_EQ(alloc.Id, 23);
    EXPECT_EQ(copied.GetAllocator().Id, 23);
    EXPECT_GE(copied.Capacity(), 0);
    EXPECT_EQ(copied.Size(), 0);

    auto moved = Array<int, TestAllocator>(std::move(alloc));
    EXPECT_EQ(alloc.Id, 0);
    EXPECT_EQ(moved.GetAllocator().Id, 23);
    EXPECT_GE(moved.Capacity(), 0);
    EXPECT_EQ(moved.Size(), 0);
}

TEST(ArrayTests, CapacityConstructor)
{
    TestAllocator alloc = TestAllocator(23);
    auto copied = Array<int, TestAllocator>(100, alloc);

    EXPECT_EQ(alloc.Id, 23);
    EXPECT_EQ(copied.GetAllocator().Id, 23);
    EXPECT_GE(copied.Capacity(), 100);
    EXPECT_EQ(copied.Size(), 0);

    auto moved = Array<int, TestAllocator>(120, std::move(alloc));
    EXPECT_EQ(alloc.Id, 0);
    EXPECT_EQ(moved.GetAllocator().Id, 23);
    EXPECT_GE(moved.Capacity(), 120);
    EXPECT_EQ(moved.Size(), 0);
}

TEST(ArrayTests, FillConstructor)
{
    TestAllocator alloc(22);
    Array<int, TestAllocator> array(6, int(97), alloc);

    EXPECT_EQ(alloc.Id, 22);
    EXPECT_EQ(array.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 6);

    for (int element : array)
        EXPECT_EQ(element, 97);

    Array<int, TestAllocator> moveAllocArray(7, 212, std::move(alloc));
    EXPECT_EQ(alloc.Id, 0);
    EXPECT_EQ(moveAllocArray.GetAllocator().Id, 22);

    EXPECT_EQ(moveAllocArray.Size(), 7);

    for (int element : moveAllocArray)
        EXPECT_EQ(element, 212);
}

TEST(ArrayTests, RangeConstructor)
{
    TestAllocator alloc(22);
    std::vector<int> vec = { 1, 2, 3, 4, 56 };

    ForwardTestContainer<int, 5> container({ 1, 2, 3, 4, 56 });
    Array<int, TestAllocator> array(container.GetBegin(), container.GetEnd(), alloc);

    EXPECT_EQ(alloc.Id, 22);
    EXPECT_EQ(array.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    Array<int, TestAllocator> moveAllocArray(container.GetBegin(), container.GetEnd(), std::move(alloc));
    EXPECT_EQ(alloc.Id, 0);
    EXPECT_EQ(moveAllocArray.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);
}

TEST(ArrayTests, InitializerListConstructor)
{
    TestAllocator alloc(22);

    std::vector<int> vec = { 1, 2, 3, 4, 56 };
    Array<int, TestAllocator> array({ 1, 2, 3, 4, 56 }, alloc);

    EXPECT_EQ(alloc.Id, 22);
    EXPECT_EQ(array.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    Array<int, TestAllocator> moveAllocArray({ 1, 2, 3, 4, 56 }, std::move(alloc));
    EXPECT_EQ(alloc.Id, 0);
    EXPECT_EQ(moveAllocArray.GetAllocator().Id, 22);

    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);
}

TEST(ArrayTests, CopyConstructor)
{
    std::vector<int> vec = { 23, 2, 65, 12, 98 };

    Array<int, TestAllocator> array({ 23, 2, 65, 12, 98 }, TestAllocator(33));
    auto copy = array;

    EXPECT_EQ(array.GetAllocator().Id, 33);
    EXPECT_EQ(copy.GetAllocator().Id, 33);

    for (std::size_t i = 0; i < array.Size(); ++i)
    {
        EXPECT_EQ(array[i], vec[i]);
        EXPECT_EQ(copy[i], vec[i]);
    }
}

TEST(ArrayTests, MoveConstructor)
{
    Array<B, TestAllocator> array({ 23, 2, 65, 12, 98 }, TestAllocator(33));
    for (B& element : array)
    {
        element.Copied = false;
        element.Moved = false;
    }

    std::vector<B> vec = { 23, 2, 65, 12, 98 };
    auto movedArray = std::move(array);

    EXPECT_EQ(movedArray.Size(), 5);
    for (std::size_t i = 0; i < movedArray.Size(); ++i)
    {
        EXPECT_FALSE(movedArray[i].Copied);
        EXPECT_EQ(movedArray[i].Id, vec[i].Id);
    }

    EXPECT_EQ(array.Size(), 0);

    EXPECT_EQ(array.GetAllocator().Id, 0);
    EXPECT_EQ(movedArray.GetAllocator().Id, 33);
}

TEST(ArrayTests, Destructor)
{
    int* allocated = (int*)32;
    int* freed = (int*)3;

    {
        auto array = Array<int, A>({ 3, 43, 451, 598, 1 }, A((void**)&allocated, (void**)&freed));
    }

    EXPECT_EQ(allocated, freed);
}

TEST(ArrayTests, CopyAssign)
{
    Array<O, TestAllocator> array({ 1, 34, 65, 234, 123 }, TestAllocator(321));
    Array<O, TestAllocator> copiedArray({ 234, 54 }, TestAllocator(32));

    array = copiedArray;

    EXPECT_EQ(array.GetAllocator(), copiedArray.GetAllocator());
    EXPECT_EQ(array.Size(), copiedArray.Size());

    for (Usize i = 0; i < array.Size(); ++i)
        EXPECT_EQ(copiedArray[i].Id, array[i].Id);
}

TEST(ArrayTests, MoveAssign)
{
    TestAllocator alloc = TestAllocator(74);
    Array<B, TestAllocator> array({ 1, 2, 3, 52, 21 }, TestAllocator(32));

    std::vector<B> vec = { 1, 25, 124, 13 };
    Array<B, TestAllocator> movedArray({ 1, 25, 124, 13 }, alloc);

    for (B& element : movedArray)
    {
        element.Copied = false;
        element.Moved = false;
    }

    array = std::move(movedArray);

    EXPECT_EQ(array.GetAllocator(), alloc);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
    {
        B& element = array[i];

        EXPECT_FALSE(element.Copied);
        EXPECT_EQ(element.Id, vec[i].Id);
    }
}

TEST(ArrayTests, InitializerListAssign)
{
    TestAllocator alloc = TestAllocator(42);

    std::vector<int> vec = { 1, 2, 3, 4, 4, 1, 2 };
    Array<int, TestAllocator> array({ 1, 2, 3, 52, 21 }, alloc);

    array = { 1, 2, 3, 4, 4, 1, 2 };

    EXPECT_EQ(array.GetAllocator(), alloc);
    EXPECT_EQ(array.Size(), 7);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    array = { /* ... */ };

    EXPECT_EQ(array.GetAllocator(), alloc);
    EXPECT_EQ(array.Size(), 0);
}

TEST(ArrayTests, Subscript)
{
    Array<int> array = { 1, 2, 3, 45, 21 };
    for (Index i = 0; i < 5; ++i)
        EXPECT_EQ(array[i], array.Data()[i]);

    EXPECT_THROW(array[5], OutOfRangeException);
}

TEST(ArrayTests, FrontBack)
{
    Array<int> array = { 1, 2, 43, 123, 451, 123 };
    Array<int> empty(53);

    EXPECT_EQ(std::addressof(array.Front()), array.Data());
    EXPECT_EQ(std::addressof(array.Back()), array.Data() + array.Size() - 1);

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

    EXPECT_EQ(std::addressof(*array.GetBegin()), array.Data());
    EXPECT_EQ(std::addressof(*array.GetReverseBegin()), std::addressof(array.Back()));

    EXPECT_EQ(array.GetEnd(), array.GetBegin() + array.Size());
    EXPECT_EQ(array.GetReverseEnd(), array.GetReverseBegin() + array.Size());
}

TEST(ArrayTests, Reserve)
{
    Array<int> array = { 32, 5, 12, 54 };
    std::vector<int> vec = { 32, 5, 12, 54 };

    Usize prevCapacity = array.Capacity();
    array.Reserve(1);

    EXPECT_GE(array.Capacity(), prevCapacity);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    array.Reserve(4);

    EXPECT_GE(array.Capacity(), prevCapacity);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    array.Reserve(12);

    EXPECT_GE(array.Capacity(), 12);
    EXPECT_EQ(array.Size(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);
}

TEST(ArrayTests, ShrinkToFit)
{
    Array<int> array = { 1, 2, 334, 532 };
    std::vector<int> vec = { 1, 2, 334, 532 };

    array.Reserve(100);

    ASSERT_GE(array.Capacity(), 100);
    array.ShrinkToFit();

    EXPECT_EQ(array.Size(), 4);
    EXPECT_EQ(array.Capacity(), 4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);
}

TEST(ArrayTests, Clear)
{
    Array<int> array = { 32, 654, 123, 32, 983, 435 };
    array.Clear();

    EXPECT_EQ(array.Size(), 0);
}

TEST(ArrayTests, InsertCopy)
{
    Array<B> array = { 32, 54, 12, 9834, 9 };
    std::vector<B> vec = { 3, 32, 54, 12, 9834, 9 };

    B value1(3);
    auto it = array.Insert(array.GetBegin(), value1);

    EXPECT_EQ(it, array.GetBegin());
    EXPECT_TRUE(array[0].Copied);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i].Id, vec[i].Id);

    std::vector<B> vec2 = { 3, 32, 54, 4, 12, 9834, 9 };

    B value2(4);
    auto it2 = array.Insert(array.GetBegin() + 3, value2);

    EXPECT_EQ(it2, array.GetBegin() + 3);
    EXPECT_TRUE(array[3].Copied);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i].Id, vec2[i].Id);

    std::vector<B> vec3 = { 3, 32, 54, 4, 12, 9834, 9, 5 };

    B value3(5);
    auto it3 = array.Insert(array.GetEnd(), value3);

    EXPECT_EQ(it3, array.GetEnd() - 1);
    EXPECT_TRUE(array.Back().Copied);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i].Id, vec3[i].Id);

    B value4(4);
    EXPECT_THROW(array.Insert(array.GetEnd() + 1, value4), OutOfRangeException);
}

TEST(ArrayTests, InsertMove)
{
    Array<B> array = { 32, 54, 12, 9834, 9 };
    std::vector<B> vec = { 3, 32, 54, 12, 9834, 9 };

    auto it = array.Insert(array.GetBegin(), B(3));

    EXPECT_EQ(it, array.GetBegin());
    EXPECT_FALSE(array[0].Copied);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i].Id, vec[i].Id);

    std::vector<B> vec2 = { 3, 32, 54, 4, 12, 9834, 9 };
    auto it2 = array.Insert(array.GetBegin() + 3, B(4));

    EXPECT_EQ(it2, array.GetBegin() + 3);
    EXPECT_FALSE(array[3].Copied);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i].Id, vec2[i].Id);

    std::vector<B> vec3 = { 3, 32, 54, 4, 12, 9834, 9, 5 };
    auto it3 = array.Insert(array.GetEnd(), 5);

    EXPECT_EQ(it3, array.GetEnd() - 1);
    EXPECT_FALSE(array.Back().Copied);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i].Id, vec3[i].Id);

    EXPECT_THROW(array.Insert(array.GetEnd() + 1, B(4)), OutOfRangeException);
}

TEST(ArrayTests, InsertFill)
{
    Array<int> array = { 54, 657, 123, 677 };
    std::vector<int> vec = { 4, 4, 4, 4, 4, 4, 4, 54, 657, 123, 677 };

    auto it = array.Insert(array.GetBegin(), 7, int(4));
    EXPECT_EQ(it, array.GetBegin());

    EXPECT_EQ(array.Size(), vec.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    std::vector<int> vec2 = { 4, 4, 4, 4, 4, 4, 4, 54, 657, 123, 677, 2, 2, 2 };
    auto it2 = array.Insert(array.GetEnd(), 3, int(2));

    EXPECT_EQ(it2, array.GetBegin() + 11);
    EXPECT_EQ(array.Size(), vec2.size());

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    std::vector<int> vec3 = { 4, 4, 4, 4, 4, 4, 4, 54, 0, 0, 657, 123, 677, 2, 2, 2 };
    auto it3 = array.Insert(array.GetBegin() + 8, 2, int(0));

    EXPECT_EQ(it3, array.GetBegin() + 8);
    EXPECT_EQ(array.Size(), vec3.size());

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec3[i]);

    auto it4 = array.Insert(array.GetBegin() + 2, 0, int(342));
    EXPECT_EQ(it4, array.GetBegin() + 2);

    EXPECT_EQ(array.Size(), vec3.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec3[i]);

    EXPECT_THROW(array.Insert(array.GetBegin() - 1, 2, int(5)), OutOfRangeException);
}

TEST(ArrayTests, InsertRange)
{
    Array<int> array;
    ForwardTestContainer<int, 3> range1({ 43, 6, 5 });

    auto it = array.Insert(array.GetBegin(), range1.GetBegin(), range1.GetEnd());

    EXPECT_EQ(it, &array.Front());
    EXPECT_EQ(array.Size(), 3);

    EXPECT_EQ(array[0], 43);
    EXPECT_EQ(array[1], 6);
    EXPECT_EQ(array[2], 5);

    ForwardTestContainer<int, 2> range2({ 3, 55 });
    auto it2 = array.Insert(array.GetBegin() + 1, range2.GetBegin(), range2.GetEnd());

    std::vector<int> vec2 = { 43, 3, 55, 6, 5 };

    EXPECT_EQ(it2, &array.Front() + 1);
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    auto it3 = array.Insert(array.GetBegin(), (int*)nullptr, (int*)nullptr);
    EXPECT_EQ(it3, array.GetBegin());
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    ForwardTestContainer<int, 2> range3({ 45, 65 });

    auto it4 = array.Insert(array.GetEnd(), range3.GetBegin(), range3.GetEnd());
    std::vector<int> vec4 = { 43, 3, 55, 6, 5, 45, 65 };

    auto expectedIt4 = array.GetEnd();
    --expectedIt4; --expectedIt4; --expectedIt4;

    EXPECT_EQ(it4, expectedIt4);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec4[i]);

    ASSERT_NE(array.GetEnd(), nullptr);

    ForwardTestContainer<int, 2> range4({ 65, 9 });
    EXPECT_THROW(array.Insert(array.GetBegin() - 1, range4.GetBegin(), range4.GetEnd()), OutOfRangeException);
}

TEST(ArrayTests, InsertInitializerList)
{
    Array<int> array;
    auto it = array.Insert(array.GetBegin(), { 43, 6, 5 });

    EXPECT_EQ(it, &array.Front());
    EXPECT_EQ(array.Size(), 3);

    EXPECT_EQ(array[0], 43);
    EXPECT_EQ(array[1], 6);
    EXPECT_EQ(array[2], 5);

    auto it2 = array.Insert(array.GetBegin() + 1, { 3, 55 });
    std::vector<int> vec2 = { 43, 3, 55, 6, 5 };

    EXPECT_EQ(it2, &array.Front() + 1);
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    auto it3 = array.Insert(array.GetBegin(), { /* ... */ });
    EXPECT_EQ(it3, array.GetBegin());
    EXPECT_EQ(array.Size(), 5);

    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec2[i]);

    auto it4 = array.Insert(array.GetEnd(), { 45, 65 });
    std::vector<int> vec4 = { 43, 3, 55, 6, 5, 45, 65 };

    auto expectedIt4 = array.GetEnd();
    --expectedIt4; --expectedIt4; --expectedIt4;

    EXPECT_EQ(it4, expectedIt4);

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
    std::vector<int> vec = { 54, 657, 123, 540 };

    array.Remove(array.GetBegin() + 3, array.GetEnd() - 1);

    EXPECT_EQ(array.Size(), vec.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    array.Remove(array.GetBegin(), array.GetBegin());

    EXPECT_EQ(array.Size(), vec.size());
    for (std::size_t i = 0; i < array.Size(); ++i)
        EXPECT_EQ(array[i], vec[i]);

    array.Remove(array.GetBegin(), array.GetEnd());
    EXPECT_EQ(array.Size(), 0);

    Array<int> array2 = { 54, 92, 172, 5 };
    EXPECT_THROW(array2.Remove(array2.GetBegin(), array2.GetEnd() + 1), OutOfRangeException);
}

TEST(ArrayTests, PushBackCopy)
{
    Array<B> array = { 453, 431 };
    B x = 534;

    array.PushBack(x);

    EXPECT_EQ(array.Size(), 3);
    EXPECT_EQ(array[0].Id, 453);
    EXPECT_EQ(array[1].Id, 431);

    EXPECT_TRUE(array[2].Copied);
    EXPECT_EQ(array[2].Id, 534);
}

TEST(ArrayTests, PushBackMove)
{
    Array<B> array = { 453, 431 };
    array.PushBack(B(534));

    EXPECT_EQ(array.Size(), 3);
    EXPECT_EQ(array[0].Id, 453);
    EXPECT_EQ(array[1].Id, 431);

    EXPECT_FALSE(array[2].Copied);
    EXPECT_EQ(array[2].Id, 534);
}

TEST(ArrayTests, EmplaceBack)
{
    Array<B> array = { 453, 431 };
    B& back = array.EmplaceBack(534);

    EXPECT_EQ(&back, &array.Back());

    EXPECT_EQ(array.Size(), 3);
    EXPECT_EQ(array[0].Id, 453);
    EXPECT_EQ(array[1].Id, 431);

    EXPECT_EQ(array[2].Id, 534);
    EXPECT_FALSE(array[2].Moved);
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
    std::vector<int> vec = { 544, 123, 12, 7 };

    int index = 0;
    for (int element : array)
    {
        EXPECT_EQ(element, vec[index]);
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
    Array<O> array1 = { 14, 23, 4343, 121 };
    Array<O> array2 = { 32, 54, 11, 43, 11 };

    array1.Swap(array2);

    EXPECT_EQ(array1.Size(), 5);
    EXPECT_EQ(array2.Size(), 4);

    EXPECT_EQ(array1[0].Id, 32);
    EXPECT_EQ(array1[1].Id, 54);
    EXPECT_EQ(array1[2].Id, 11);
    EXPECT_EQ(array1[3].Id, 43);
    EXPECT_EQ(array1[4].Id, 11);

    EXPECT_EQ(array2[0].Id, 14);
    EXPECT_EQ(array2[1].Id, 23);
    EXPECT_EQ(array2[2].Id, 4343);
    EXPECT_EQ(array2[3].Id, 121);
}
