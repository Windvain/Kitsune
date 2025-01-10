#include <gtest/gtest.h>

#include "TestContainer.h"
#include "IteratorWrappers.h"

#include "Foundation/Containers/Array.h"

namespace ArrayTesting
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

    bool operator==(const A& /* a1 */, const A& /* a2 */) { return true; }

    class TestAllocator : public Kitsune::GlobalAllocator
    {
    public:
        TestAllocator() = default;
        explicit TestAllocator(int id) : ID(id) { /* ... */ }

        TestAllocator(const TestAllocator&) = default;
        TestAllocator(TestAllocator&& a)
        {
            ID = std::exchange(a.ID, 0);
        }

        ~TestAllocator() = default;

    public:
        TestAllocator& operator=(const TestAllocator&) = default;
        TestAllocator& operator=(TestAllocator&& a) { ID = std::exchange(a.ID, 0); return *this; }

    public:
        bool operator==(const TestAllocator& alloc) const { return (ID == alloc.ID); }

    public:
        int ID = 0;
    };

    class O
    {
    public:
        O() = default;
        O(int id) : ID(id) { /* ... */ }

        O(const O&) = default;
        O(O&& obj) { ID = std::exchange(obj.ID, 0); }
        ~O() = default;

    public:
        O& operator=(const O&) = default;
        O& operator=(O&& obj) { ID = std::exchange(obj.ID, 0); return *this; }

    public:
        int ID = 0;
    };
}

using namespace Kitsune;
using namespace ArrayTesting;
using Testing::TestContainer, Testing::ForwardIteratorWrapper;

TEST(ArrayTests, DefaultCtor)
{
    Array<int> arr;

    EXPECT_EQ(arr.Size(), 0);
    EXPECT_EQ(arr.Capacity(), 0);
    EXPECT_EQ(arr.Data(), nullptr);
}

TEST(ArrayTests, AllocatorCtor)
{
    TestAllocator alloc = TestAllocator(23);
    auto copied = Array<int, TestAllocator>(alloc);

    EXPECT_EQ(alloc.ID, 23);
    EXPECT_EQ(copied.GetAllocator().ID, 23);
    EXPECT_GE(copied.Capacity(), 0);
    EXPECT_EQ(copied.Size(), 0);

    auto moved = Array<int, TestAllocator>(std::move(alloc));
    EXPECT_EQ(alloc.ID, 0);
    EXPECT_EQ(moved.GetAllocator().ID, 23);
    EXPECT_GE(moved.Capacity(), 0);
    EXPECT_EQ(moved.Size(), 0);
}

TEST(ArrayTests, CapacityCtor)
{
    TestAllocator alloc = TestAllocator(23);
    auto copied = Array<int, TestAllocator>(100, alloc);

    EXPECT_EQ(alloc.ID, 23);
    EXPECT_EQ(copied.GetAllocator().ID, 23);
    EXPECT_GE(copied.Capacity(), 100);
    EXPECT_EQ(copied.Size(), 0);

    auto moved = Array<int, TestAllocator>(120, std::move(alloc));
    EXPECT_EQ(alloc.ID, 0);
    EXPECT_EQ(moved.GetAllocator().ID, 23);
    EXPECT_GE(moved.Capacity(), 120);
    EXPECT_EQ(moved.Size(), 0);
}

TEST(ArrayTests, FillCtor)
{
    TestAllocator alloc = TestAllocator(23);
    auto copied = Array<int, TestAllocator>(100, 5, alloc);

    EXPECT_EQ(alloc.ID, 23);
    EXPECT_EQ(copied.GetAllocator().ID, 23);
    EXPECT_EQ(copied.Size(), 100);

    auto moved = Array<int, TestAllocator>(120, 65, std::move(alloc));
    EXPECT_EQ(alloc.ID, 0);
    EXPECT_EQ(moved.GetAllocator().ID, 23);
    EXPECT_GE(moved.Size(), 120);

    for (int i = 0; i < 100; ++i)
        EXPECT_EQ(copied.Data()[i], 5);

    for (int i = 0; i < 120; ++i)
        EXPECT_EQ(moved.Data()[i], 65);
}

TEST(ArrayTests, RangeCtor)
{
    int beg[5] = { 1, 3, 56, 23498, 2346 };

    TestAllocator alloc = TestAllocator(23);
    TestContainer<ForwardIteratorWrapper<int>> cont(beg, beg + 5);

    auto copied = Array<int, TestAllocator>(cont.Begin, cont.End, alloc);
    EXPECT_EQ(alloc.ID, 23);
    EXPECT_EQ(copied.GetAllocator().ID, 23);
    EXPECT_EQ(copied.Size(), 5);

    auto moved = Array<int, TestAllocator>(cont.Begin, cont.End, std::move(alloc));
    EXPECT_EQ(alloc.ID, 0);
    EXPECT_EQ(moved.GetAllocator().ID, 23);
    EXPECT_EQ(moved.Size(), 5);

    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(beg[i], copied.Data()[i]);

    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(beg[i], moved.Data()[i]);
}

TEST(ArrayTests, InitializerListCtor)
{
    TestAllocator alloc = TestAllocator(23);

    auto copied = Array<int, TestAllocator>({ 1, 2, 3, 4, 56 }, alloc);
    EXPECT_EQ(alloc.ID, 23);
    EXPECT_EQ(copied.GetAllocator().ID, 23);
    EXPECT_EQ(copied.Size(), 5);

    auto moved = Array<int, TestAllocator>({ 92, 1823, 32, 1 }, std::move(alloc));
    EXPECT_EQ(alloc.ID, 0);
    EXPECT_EQ(moved.GetAllocator().ID, 23);
    EXPECT_EQ(moved.Size(), 4);

    EXPECT_EQ(copied.Data()[0], 1);
    EXPECT_EQ(copied.Data()[1], 2);
    EXPECT_EQ(copied.Data()[2], 3);
    EXPECT_EQ(copied.Data()[3], 4);
    EXPECT_EQ(copied.Data()[4], 56);

    EXPECT_EQ(moved.Data()[0], 92);
    EXPECT_EQ(moved.Data()[1], 1823);
    EXPECT_EQ(moved.Data()[2], 32);
    EXPECT_EQ(moved.Data()[3], 1);
}

TEST(ArrayTests, CopyCtor)
{
    auto arr = Array<O, TestAllocator>({ 23, 2, 65, 12, 98 }, TestAllocator(33));
    auto copy = arr;

    EXPECT_EQ(arr.GetAllocator().ID, 33);
    EXPECT_EQ(copy.Size(), 5);
    EXPECT_EQ(copy.GetAllocator().ID, 33);

    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(copy.Data()[i].ID, arr.Data()[i].ID);
}

TEST(ArrayTests, MoveCtor)
{
    auto arr = Array<O, TestAllocator>({ 23, 2, 65, 12, 98 }, TestAllocator(33));
    auto move = std::move(arr);

    EXPECT_EQ(arr.GetAllocator().ID, 0);
    EXPECT_EQ(arr.Size(), 0);

    EXPECT_EQ(move.Size(), 5);
    EXPECT_EQ(move.GetAllocator().ID, 33);

    EXPECT_EQ(move.Data()[0].ID, 23);
    EXPECT_EQ(move.Data()[1].ID, 2);
    EXPECT_EQ(move.Data()[2].ID, 65);
    EXPECT_EQ(move.Data()[3].ID, 12);
    EXPECT_EQ(move.Data()[4].ID, 98);
}

TEST(ArrayTests, Destructor)
{
    int* allocated = (int*)32;
    int* freed = (int*)3;

    {
        auto arr = Array<int, A>({ 3, 43, 451, 598, 1 }, A((void**)&allocated, (void**)&freed));
    }

    EXPECT_EQ(allocated, freed);
}

TEST(ArrayTests, CopyAssign)
{
    auto arr = Array<O, TestAllocator>({ 1, 34, 65, 234, 123 }, TestAllocator(321));
    auto copy = Array<O, TestAllocator>({ 234, 54 }, TestAllocator(32));

    copy = arr;

    EXPECT_EQ(arr.GetAllocator(), copy.GetAllocator());
    EXPECT_EQ(copy.Size(), arr.Size());

    for (Usize i = 0; i < 5; ++i)
        EXPECT_EQ(copy.Data()[i].ID, arr.Data()[i].ID);
}

TEST(ArrayTests, MoveAssign)
{
    int rawArray[5] = { 1, 2, 3, 52, 21 };
    TestContainer<ForwardIteratorWrapper<int>> cont(rawArray, rawArray + 5);

    TestAllocator alloc = TestAllocator(32);
    auto arr = Array<int, TestAllocator>(cont.Begin, cont.End, alloc);
    auto mv = Array<int, TestAllocator>({ 1, 2, 12343 }, TestAllocator(74));

    mv = std::move(arr);

    EXPECT_EQ(mv.GetAllocator(), alloc);
    EXPECT_EQ(mv.Size(), 5);

    for (int i = 0; i < 5; ++i)
        EXPECT_EQ(mv.Data()[i], rawArray[i]);
}

TEST(ArrayTests, InitializerListAssign)
{
    TestAllocator alloc = TestAllocator(42);
    auto arr = Array<int, TestAllocator>({ 1, 2, 3, 52, 21 }, alloc);

    arr = { 1, 2, 3, 4, 4, 1, 2 };

    EXPECT_EQ(arr.GetAllocator(), alloc);
    EXPECT_EQ(arr.Size(), 7);

    EXPECT_EQ(arr.Data()[0], 1);
    EXPECT_EQ(arr.Data()[1], 2);
    EXPECT_EQ(arr.Data()[2], 3);
    EXPECT_EQ(arr.Data()[3], 4);
    EXPECT_EQ(arr.Data()[4], 4);
    EXPECT_EQ(arr.Data()[5], 1);
    EXPECT_EQ(arr.Data()[6], 2);
}

TEST(ArrayTests, Subscript)
{
    Array<int> arr = { 1, 2, 3, 45, 21 };
    for (Index i = 0; i < 5; ++i)
        EXPECT_EQ(arr[i], arr.Data()[i]);
}

TEST(ArrayTests, FrontBack)
{
    Array<int> arr = { 1, 2, 43, 123, 451, 123 };
    EXPECT_EQ(arr.Front(), arr[0]);
    EXPECT_EQ(arr.Back(), arr[5]);
}

TEST(ArrayTests, Data)
{
    Array<int> arr = { 1, 2, 32, 312, 231 };
    EXPECT_EQ(arr.Data(), std::addressof(arr.Front()));
}

TEST(ArrayTests, GetAllocator)
{
    TestAllocator alloc = TestAllocator(982);
    auto arr = Array<int, TestAllocator>({ 123, 123, 534, 13 }, alloc);

    EXPECT_EQ(arr.GetAllocator(), alloc);
}

TEST(ArrayTests, Size)
{
    Array<int> arr = { 123, 232, 563 };
    EXPECT_EQ(arr.Size(), 3);
}

TEST(ArrayTests, Capacity)
{
    Array<int> empty(231);
    Array<int> arr = { 123, 2398, 123987 };

    EXPECT_GE(empty.Capacity(), 231);
    EXPECT_GE(arr.Capacity(), 3);
}

TEST(ArrayTests, IsEmpty)
{
    Array<int> arr = { 1, 2, 323, 432, 123 };
    Array<int> empty(52);

    EXPECT_FALSE(arr.IsEmpty());
    EXPECT_TRUE(empty.IsEmpty());
}

TEST(ArrayTests, Iterators)
{
    Array<int> arr = { 3, 32, 12, 9, 7 };
    EXPECT_EQ(*arr.GetBegin(), arr[0]);
    EXPECT_EQ(arr.GetEnd(), arr.GetBegin() + 5);

    EXPECT_EQ(*arr.GetReverseBegin(), arr[4]);
    EXPECT_EQ(arr.GetReverseEnd(), arr.GetReverseBegin() + 5);
}

TEST(ArrayTests, Reserve)
{
    Array<int> arr = { 123, 453, 123, 8342 };
    Usize prevCap = arr.Capacity();

    arr.Reserve(1);     // Smaller than current capacity.
    EXPECT_EQ(arr.Capacity(), prevCap);
    EXPECT_EQ(arr.Size(), 4);

    arr.Reserve(100);
    EXPECT_GE(arr.Capacity(), 100);
    EXPECT_EQ(arr.Size(), 4);
}

TEST(ArrayTests, Shrink)
{
    Array<int> arr = { 1, 2, 334, 532 };
    arr.Reserve(100);       // capacity == 100.

    arr.ShrinkToFit();
    EXPECT_EQ(arr.Size(), 4);
    EXPECT_EQ(arr.Size(), arr.Capacity());
}

TEST(ArrayTests, Clear)
{
    Array<int> arr = { 32, 654, 123, 32, 983, 435 };
    arr.Clear();

    EXPECT_EQ(arr.Size(), 0);
    EXPECT_EQ(arr.Capacity(), 0);
}

TEST(ArrayTests, InsertCopy)
{
    Array<O> arr = { 32, 54, 12, 9834, 9 };
    O x = 75;

    auto it = arr.Insert(arr.GetBegin() + 3, x);

    EXPECT_EQ(it, arr.GetBegin() + 3);
    EXPECT_EQ(arr.Size(), 6);

    EXPECT_EQ(arr[0].ID, 32);
    EXPECT_EQ(arr[1].ID, 54);
    EXPECT_EQ(arr[2].ID, 12);
    EXPECT_EQ(arr[3].ID, 75);
    EXPECT_EQ(arr[4].ID, 9834);
    EXPECT_EQ(arr[5].ID, 9);
}

TEST(ArrayTests, InsertMove)
{
    Array<int> arr = { 32, 54, 12, 9834, 9 };
    auto it = arr.Insert(arr.GetBegin() + 3, 75);

    EXPECT_EQ(it, arr.GetBegin() + 3);
    EXPECT_EQ(arr.Size(), 6);

    EXPECT_EQ(arr[0], 32);
    EXPECT_EQ(arr[1], 54);
    EXPECT_EQ(arr[2], 12);
    EXPECT_EQ(arr[3], 75);
    EXPECT_EQ(arr[4], 9834);
    EXPECT_EQ(arr[5], 9);
}

TEST(ArrayTests, InsertFill)
{
    Array<int> arr = { 54, 657, 123, 677 };
    auto it = arr.Insert(arr.GetBegin(), 7, int(4));
    auto it2 = arr.Insert(arr.GetEnd(), 1, int(2));

    EXPECT_EQ(it, arr.GetBegin());
    EXPECT_EQ(it2, arr.GetEnd() - 1);

    EXPECT_EQ(arr.Size(), 12);
    EXPECT_EQ(arr[0], 4);
    EXPECT_EQ(arr[1], 4);
    EXPECT_EQ(arr[2], 4);
    EXPECT_EQ(arr[3], 4);
    EXPECT_EQ(arr[4], 4);
    EXPECT_EQ(arr[5], 4);
    EXPECT_EQ(arr[6], 4);
    EXPECT_EQ(arr[7], 54);
    EXPECT_EQ(arr[8], 657);
    EXPECT_EQ(arr[9], 123);
    EXPECT_EQ(arr[10], 677);
    EXPECT_EQ(arr[11], 2);
}

TEST(ArrayTests, InsertRange)
{
    Array<int> arr = { 54, 657, 123, 677 };
    int rngArray[2] = { 43, 6 };

    TestContainer<ForwardIteratorWrapper<int>> rng(rngArray, rngArray + 2);
    auto it = arr.Insert(arr.GetBegin(), rng.Begin, rng.End);

    EXPECT_EQ(it, arr.GetBegin());
    EXPECT_EQ(arr.Size(), 6);
    EXPECT_EQ(arr[0], 43);
    EXPECT_EQ(arr[1], 6);
    EXPECT_EQ(arr[2], 54);
    EXPECT_EQ(arr[3], 657);
    EXPECT_EQ(arr[4], 123);
    EXPECT_EQ(arr[5], 677);
}

TEST(ArrayTests, InsertInitializerList)
{
    Array<int> arr = { 54, 657, 123, 677 };
    auto it = arr.Insert(arr.GetBegin(), { 43, 6 });

    EXPECT_EQ(it, arr.GetBegin());
    EXPECT_EQ(arr.Size(), 6);
    EXPECT_EQ(arr[0], 43);
    EXPECT_EQ(arr[1], 6);
    EXPECT_EQ(arr[2], 54);
    EXPECT_EQ(arr[3], 657);
    EXPECT_EQ(arr[4], 123);
    EXPECT_EQ(arr[5], 677);
}

TEST(ArrayTests, Remove)
{
    Array<int> arr = { 54, 657, 123, 677 };
    arr.Remove(arr.GetBegin() + 1);

    EXPECT_EQ(arr.Size(), 3);

    EXPECT_EQ(arr[0], 54);
    EXPECT_EQ(arr[1], 123);
    EXPECT_EQ(arr[2], 677);
}

TEST(ArrayTests, RemoveRange)
{
    Array<int> arr = { 54, 657, 123, 677 };
    arr.Remove(arr.GetBegin() + 1, arr.GetEnd() - 1);

    EXPECT_EQ(arr.Size(), 2);

    EXPECT_EQ(arr[0], 54);
    EXPECT_EQ(arr[1], 677);
}

TEST(ArrayTests, PushBackCopy)
{
    Array<int> arr = { 453, 431 };
    int x = 534;

    arr.PushBack(x);

    EXPECT_EQ(arr.Size(), 3);
    EXPECT_EQ(arr[0], 453);
    EXPECT_EQ(arr[1], 431);
    EXPECT_EQ(arr[2], 534);
}

TEST(ArrayTests, PushBackMove)
{
    Array<int> arr = { 453, 431 };
    arr.PushBack(534);

    EXPECT_EQ(arr.Size(), 3);
    EXPECT_EQ(arr[0], 453);
    EXPECT_EQ(arr[1], 431);
    EXPECT_EQ(arr[2], 534);
}

TEST(ArrayTests, EmplaceBack)
{
    Array<int> arr = { 453, 431 };
    arr.EmplaceBack(534);

    EXPECT_EQ(arr.Size(), 3);
    EXPECT_EQ(arr[0], 453);
    EXPECT_EQ(arr[1], 431);
    EXPECT_EQ(arr[2], 534);
}

TEST(ArrayTests, PopBack)
{
    Array<int> arr = { 43, 675, 123 };
    arr.PopBack();

    EXPECT_EQ(arr.Size(), 2);
    EXPECT_EQ(arr[0], 43);
    EXPECT_EQ(arr[1], 675);
}

TEST(ArrayTests, RangedForLoop)
{
    Array<int> arr = { 2, 3, 4, 5 };
    int i = 2;
    for (int& e : arr)
        EXPECT_EQ(e, i++);
}

TEST(ArrayTests, Equal)
{
    Array<int> arr = { 12, 23, 23, 54, 64 };
    Array<int> arr2 = { 12, 23, 23, 54, 64 };

    Array<int> diff = { 12, 23, 23, 54, 64, 72 };

    EXPECT_TRUE(arr == arr2);
    EXPECT_FALSE(arr == diff);

    EXPECT_FALSE(arr != arr2);
    EXPECT_TRUE(arr != diff);
}
