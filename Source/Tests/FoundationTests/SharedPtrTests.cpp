#include <gtest/gtest.h>

#include <algorithm>
#include "Foundation/Memory/SharedPtr.h"

namespace SharedPtrTesting
{
    template<typename T>
    class E
    {
    public:
        using ValueType = T;

        E() = default;
        E(T** deletedPtr, int id = 0)
            : ID(id), m_DeletedPtr(deletedPtr)
        {
        }

        E(const E& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            ID = deleter.ID;
        }

        E(E&& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            ID = deleter.ID;

            deleter.Moved = true;

            deleter.m_DeletedPtr = nullptr;
            deleter.ID = 0;
        }

        ~E() = default;

    public:
        E& operator=(const E& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            ID = deleter.ID;

            return *this;
        }

        template<typename U>
        E& operator=(const E<U>& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            ID = deleter.ID;

            return *this;
        }

        E& operator=(E&& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            ID = deleter.ID;

            deleter.m_DeletedPtr = nullptr;
            deleter.ID = 0;

            return *this;
        }

        template<typename U>
        E& operator=(E<U>&& deleter)
        {
            m_DeletedPtr = (T**)deleter.m_DeletedPtr;
            ID = deleter.ID;

            deleter.m_DeletedPtr = nullptr;
            deleter.ID = 0;

            return *this;
        }

        void operator()(T* ptr)
        {
            if (m_DeletedPtr)
                *m_DeletedPtr = ptr;

            Kitsune::Memory::Delete(ptr);
        }

    public:
        int ID = 0;
        bool Moved = false;

    private:
        template<typename U>
        friend class E;

        T** m_DeletedPtr;
    };

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

    class B { /* ... */ };
    class C : public B
    {
    };
}

using namespace Kitsune;
using namespace SharedPtrTesting;

TEST(SharedPtrTests, DefaultNullptrCtor)
{
    SharedPtr<int> ptr;
    SharedPtr<int> null = nullptr;

    EXPECT_EQ(ptr.Get(), nullptr);
    EXPECT_EQ(null.Get(), nullptr);
}

TEST(SharedPtrTests, PointerCtor)
{
    int* raw = Memory::New<int>(10);
    auto ptr = SharedPtr<int>(raw);

    EXPECT_EQ(ptr.Get(), raw);
}

TEST(SharedPtrTests, DeleterCtor)
{
    int *raw1, *raw2;
    int *deleted1 = nullptr, *deleted2 = nullptr;

    raw1 = Memory::New<int>();
    raw2 = Memory::New<int>();

    {
        E<int> movedDeleter = E<int>(&deleted1);
        E<int> copiedDel = E<int>(&deleted2);

        auto moved = SharedPtr<int>(raw1, std::move(movedDeleter));
        auto copied = SharedPtr<int>(raw2, copiedDel);

        EXPECT_TRUE(movedDeleter.Moved);
        EXPECT_FALSE(copiedDel.Moved);
    }

    EXPECT_EQ(raw1, deleted1);
    EXPECT_EQ(raw2, deleted2);
}

TEST(SharedPtrTests, NullptrDeleterCtor)
{
    int *deleted1 = nullptr, *deleted2 = nullptr;

    {
        E<int> movedDeleter = E<int>(&deleted1);
        E<int> copiedDel = E<int>(&deleted2);

        auto moved = SharedPtr<int>(nullptr, std::move(movedDeleter));
        auto copied = SharedPtr<int>(nullptr, copiedDel);

        EXPECT_TRUE(movedDeleter.Moved);
        EXPECT_FALSE(copiedDel.Moved);
    }

    EXPECT_EQ(deleted1, nullptr);
    EXPECT_EQ(deleted2, nullptr);
}

TEST(SharedPtrTests, SameOpDeleterAllocCtor)
{
    int *raw1, *raw2;
    int *deleted1 = nullptr, *deleted2 = nullptr;

    void *allocated = nullptr, *freed = nullptr;
    void *allocated2 = nullptr, *freed2 = nullptr;

    raw1 = Memory::New<int>();
    raw2 = Memory::New<int>();

    {
        E<int> movedDeleter = E<int>(&deleted1);
        E<int> copiedDel = E<int>(&deleted2);

        A alloc1 = A(&allocated, &freed);
        A alloc2 = A(&allocated2, &freed2);

        auto moved = SharedPtr<int>(raw1, std::move(movedDeleter), std::move(alloc1));
        auto copied = SharedPtr<int>(raw2, copiedDel, alloc2);

        EXPECT_TRUE(movedDeleter.Moved);
        EXPECT_TRUE(alloc1.Moved);

        EXPECT_FALSE(copiedDel.Moved);
        EXPECT_FALSE(alloc2.Moved);
    }

    EXPECT_EQ(allocated, freed);
    EXPECT_EQ(allocated2, freed2);

    EXPECT_EQ(raw1, deleted1);
    EXPECT_EQ(raw2, deleted2);
}

TEST(SharedPtrTests, DiffOpDeleterAllocCtor)
{
    int *raw1, *raw2;
    int *deleted1 = nullptr, *deleted2 = nullptr;

    void *allocated = nullptr, *freed = nullptr;
    void *allocated2 = nullptr, *freed2 = nullptr;

    raw1 = Memory::New<int>();
    raw2 = Memory::New<int>();

    {
        E<int> movedDeleter = E<int>(&deleted1);
        E<int> copiedDel = E<int>(&deleted2);

        A alloc1 = A(&allocated, &freed);
        A alloc2 = A(&allocated2, &freed2);

        auto moved = SharedPtr<int>(raw1, std::move(movedDeleter), alloc1);
        auto copied = SharedPtr<int>(raw2, copiedDel, std::move(alloc2));

        EXPECT_TRUE(movedDeleter.Moved);
        EXPECT_TRUE(alloc2.Moved);

        EXPECT_FALSE(alloc1.Moved);
        EXPECT_FALSE(copiedDel.Moved);
    }

    EXPECT_EQ(allocated, freed);
    EXPECT_EQ(allocated2, freed2);

    EXPECT_EQ(raw1, deleted1);
    EXPECT_EQ(raw2, deleted2);
}

TEST(SharedPtrTests, NullptrSameOpDeleterAllocCtor)
{
    int *deleted1 = (int*)0xDEAD, *deleted2 = (int*)0xDEAD;

    void *allocated = nullptr, *freed = nullptr;
    void *allocated2 = nullptr, *freed2 = nullptr;

    {
        E<int> movedDeleter = E<int>(&deleted1);
        E<int> copiedDel = E<int>(&deleted2);

        A alloc1 = A(&allocated, &freed);
        A alloc2 = A(&allocated2, &freed2);

        auto moved = SharedPtr<int>(nullptr, std::move(movedDeleter), std::move(alloc1));
        auto copied = SharedPtr<int>(nullptr, copiedDel, alloc2);

        EXPECT_TRUE(movedDeleter.Moved);
        EXPECT_TRUE(alloc1.Moved);

        EXPECT_FALSE(copiedDel.Moved);
        EXPECT_FALSE(alloc2.Moved);
    }

    EXPECT_EQ(allocated, freed);
    EXPECT_EQ(allocated2, freed2);

    EXPECT_EQ((int*)0xDEAD, deleted1);      // Deleter doesn't get called.
    EXPECT_EQ((int*)0xDEAD, deleted2);      // Deleter doesn't get called.
}

TEST(SharedPtrTests, NullptrDiffOpDeleterAllocCtor)
{
    int *deleted1 = (int*)0xDEAD, *deleted2 = (int*)0xDEAD;

    void *allocated = nullptr, *freed = nullptr;
    void *allocated2 = nullptr, *freed2 = nullptr;

    {
        E<int> movedDeleter = E<int>(&deleted1);
        E<int> copiedDel = E<int>(&deleted2);

        A alloc1 = A(&allocated, &freed);
        A alloc2 = A(&allocated2, &freed2);

        auto moved = SharedPtr<int>(nullptr, std::move(movedDeleter), alloc1);
        auto copied = SharedPtr<int>(nullptr, copiedDel, std::move(alloc2));

        EXPECT_TRUE(movedDeleter.Moved);
        EXPECT_TRUE(alloc2.Moved);

        EXPECT_FALSE(alloc1.Moved);
        EXPECT_FALSE(copiedDel.Moved);
    }

    EXPECT_EQ(allocated, freed);
    EXPECT_EQ(allocated2, freed2);

    EXPECT_EQ((int*)0xDEAD, deleted1);      // Deleter doesn't get called.
    EXPECT_EQ((int*)0xDEAD, deleted2);      // Deleter doesn't get called.
}

TEST(SharedPtrTests, CopyCtor)
{
    auto null = SharedPtr<int>();
    auto ptr = MakeShared<int>(5);

    auto copyNull = null;
    auto copyPtr = ptr;

    EXPECT_EQ(copyNull.Get(), nullptr);
    EXPECT_EQ(copyPtr.Get(), ptr.Get());

    EXPECT_EQ(copyNull.GetCount(), 0);
    EXPECT_EQ(copyPtr.GetCount(), 2);
}

TEST(SharedPtrTests, TemplatedCopyCtor)
{
    auto null = SharedPtr<C>();
    auto ptr = MakeShared<C>();

    SharedPtr<B> copyNull = null;
    SharedPtr<B> copyPtr = ptr;

    EXPECT_EQ(copyNull.Get(), nullptr);
    EXPECT_EQ(copyPtr.Get(), ptr.Get());

    EXPECT_EQ(copyNull.GetCount(), 0);
    EXPECT_EQ(copyPtr.GetCount(), 2);
}

TEST(SharedPtrTests, MoveCtor)
{
    int* raw = Memory::New<int>(5);

    auto null = SharedPtr<int>();
    auto ptr = SharedPtr<int>(raw);

    auto moveNull = std::move(null);
    auto movePtr = std::move(ptr);

    EXPECT_EQ(moveNull.Get(), nullptr);
    EXPECT_EQ(movePtr.Get(), raw);

    EXPECT_EQ(moveNull.GetCount(), 0);
    EXPECT_EQ(movePtr.GetCount(), 1);
}

TEST(SharedPtrTests, TemplatedMoveCtor)
{
    C* raw = Memory::New<C>();

    auto null = SharedPtr<C>();
    auto ptr = SharedPtr<C>(raw);

    SharedPtr<B> moveNull = std::move(null);
    SharedPtr<B> movePtr = std::move(ptr);

    EXPECT_EQ(moveNull.Get(), nullptr);
    EXPECT_EQ(null.GetCount(), 0);
    EXPECT_EQ(movePtr.Get(), raw);

    EXPECT_EQ(moveNull.GetCount(), 0);
    EXPECT_EQ(ptr.GetCount(), 0);
    EXPECT_EQ(movePtr.GetCount(), 1);
}

TEST(SharedPtrTests, WeakPtrCtor)
{
    SharedPtr<int> ptr = MakeShared<int>(1);
    SharedPtr<int> copy;

    {
        auto weak = WeakPtr<int>(ptr);
        EXPECT_EQ(ptr.GetCount(), 1);

        copy = SharedPtr<int>(weak);
        EXPECT_EQ(ptr.GetCount(), 2);
        EXPECT_EQ(copy.Get(), ptr.Get());
    }

    EXPECT_EQ(ptr.GetCount(), 2);
}

TEST(SharedPtrTests, ScopedPtrCtor)
{
    C* raw = Memory::New<C>();
    C* deleted = nullptr;

    auto ptr = ScopedPtr<C, E<C>>(raw, E<C>(&deleted));
    SharedPtr<B> movePtr = std::move(ptr);

    EXPECT_EQ(movePtr.Get(), raw);
    EXPECT_EQ(movePtr.GetCount(), 1);
    EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(SharedPtrTests, Destructor)
{
    int* raw = Memory::New<int>(5);
    int* deleted = nullptr;

    {
        E<int> deleter = E<int>(&deleted);
        SharedPtr<int> ptr = SharedPtr<int>(raw, std::move(deleter));

        KITSUNE_UNUSED(ptr);
    }

    EXPECT_EQ(raw, deleted);
}

TEST(SharedPtrTests, CopyAssign)
{
    int *raw1, *raw2;
    int *deleted = nullptr, *deleted2 = nullptr;

    raw1 = Memory::New<int>(5);
    raw2 = Memory::New<int>(10);

    {
        auto ptr = SharedPtr<int>(raw1, E<int>(&deleted2));
        auto copy = SharedPtr<int>(raw2, E<int>(&deleted));

        copy = ptr;

        EXPECT_EQ(deleted, raw2);
        EXPECT_EQ(copy.Get(), ptr.Get());
    }

    EXPECT_EQ(deleted2, raw1);
}

TEST(SharedPtrTests, TemplateCopyAssign)
{
    C* raw1; B* raw2;
    B* deleted = nullptr;
    C* deleted2 = nullptr;

    raw1 = Memory::New<C>();
    raw2 = Memory::New<B>();

    {
        auto ptr = SharedPtr<C>(raw1, E<C>(&deleted2));
        auto copy = SharedPtr<B>(raw2, E<B>(&deleted));

        copy = ptr;

        EXPECT_EQ(deleted, raw2);
        EXPECT_EQ(copy.Get(), ptr.Get());
    }

    EXPECT_EQ(deleted2, raw1);
}

TEST(SharedPtrTests, MoveAssign)
{
    int *raw1, *raw2;
    int *deleted = nullptr, *deleted2 = nullptr;

    raw1 = Memory::New<int>(5);
    raw2 = Memory::New<int>(10);

    {
        auto ptr = SharedPtr<int>(raw1, E<int>(&deleted2));
        auto move = SharedPtr<int>(raw2, E<int>(&deleted));

        move = std::move(ptr);

        EXPECT_EQ(deleted, raw2);
        EXPECT_EQ(move.Get(), raw1);
        EXPECT_EQ(ptr.GetCount(), 0);
    }

    EXPECT_EQ(deleted2, raw1);
}

TEST(SharedPtrTests, TemplatedMoveAssign)
{
    C* raw1; B* raw2;
    B* deleted = nullptr; C* deleted2 = nullptr;

    raw1 = Memory::New<C>();
    raw2 = Memory::New<B>();

    {
        auto ptr = SharedPtr<C>(raw1, E<C>(&deleted2));
        auto move = SharedPtr<B>(raw2, E<B>(&deleted));

        move = std::move(ptr);

        EXPECT_EQ(deleted, raw2);
        EXPECT_EQ(move.Get(), raw1);
        EXPECT_EQ(ptr.GetCount(), 0);
    }

    EXPECT_EQ(deleted2, raw1);
}

TEST(SharedPtrTests, ScopedPtrAssign)
{
    C* raw1; B* raw2;
    B* deleted = nullptr; C* deleted2 = nullptr;

    raw1 = Memory::New<C>();
    raw2 = Memory::New<B>();

    {
        auto ptr = ScopedPtr<C, E<C>>(raw1, E<C>(&deleted2));
        auto move = SharedPtr<B>(raw2, E<B>(&deleted));

        move = std::move(ptr);

        EXPECT_EQ(deleted, raw2);
        EXPECT_EQ(move.Get(), raw1);
        EXPECT_EQ(ptr.Get(), nullptr);
    }

    EXPECT_EQ(deleted2, raw1);
}

TEST(SharedPtrTests, Dereference)
{
    SharedPtr<int> ptr = MakeShared<int>(5);
    EXPECT_EQ(&*ptr, ptr.Get());
}

TEST(SharedPtrTests, Boolean)
{
    auto ptr = MakeShared<int>(5);
    auto empty = SharedPtr<int>();

    EXPECT_TRUE((bool)ptr);
    EXPECT_FALSE((bool)empty);
}

TEST(SharedPtrTests, GetCount)
{
    auto ptr1 = MakeShared<int>(2);
    auto ptr2 = ptr1;
    auto ptr3 = ptr2;
    auto ptr4 = ptr3;

    EXPECT_EQ(ptr1.GetCount(), 4);
}

TEST(SharedPtrTests, Comparison)
{
    int *mem1, *mem2, *mem3;
    mem1 = Memory::New<int>();
    mem2 = Memory::New<int>();
    mem3 = Memory::New<int>();

    std::vector<int*> arr = { mem1, mem2, mem3 };

    int* max = KITSUNE_MAX(KITSUNE_MAX(mem1, mem2), mem3);
    int* min = KITSUNE_MIN(KITSUNE_MIN(mem1, mem2), mem3);

    arr.erase(std::find(arr.begin(), arr.end(), max));
    arr.erase(std::find(arr.begin(), arr.end(), min));

    auto lesser  = SharedPtr<int>(min);
    auto mid = SharedPtr<int>(arr[0]);
    auto larger  = SharedPtr<int>(max);

    int* null = nullptr;

    EXPECT_EQ(mid == nullptr, mid.Get() == null);
    EXPECT_EQ(mid != nullptr, mid.Get() != null);
    EXPECT_EQ(mid < nullptr, mid.Get() < null);
    EXPECT_EQ(mid > nullptr, mid.Get() > null);
    EXPECT_EQ(mid <= nullptr, mid.Get() <= null);
    EXPECT_EQ(mid >= nullptr, mid.Get() >= null);

    EXPECT_EQ(nullptr == mid, null == mid.Get());
    EXPECT_EQ(nullptr != mid, null != mid.Get());
    EXPECT_EQ(nullptr < mid, null < mid.Get());
    EXPECT_EQ(nullptr > mid, null > mid.Get());
    EXPECT_EQ(nullptr <= mid, null <= mid.Get());
    EXPECT_EQ(nullptr >= mid, null >= mid.Get());

    EXPECT_TRUE(lesser < mid);
    EXPECT_TRUE(mid < larger);
    EXPECT_FALSE(larger < mid);
    EXPECT_FALSE(mid < lesser);

    EXPECT_TRUE(larger > mid);
    EXPECT_TRUE(mid > lesser);
    EXPECT_FALSE(mid > larger);
    EXPECT_FALSE(lesser > mid);

    EXPECT_TRUE(lesser <= mid);
    EXPECT_TRUE(mid <= mid);
    EXPECT_TRUE(mid <= larger);
    EXPECT_FALSE(mid <= lesser);
    EXPECT_FALSE(larger <= mid);

    EXPECT_TRUE(larger >= mid);
    EXPECT_TRUE(mid >= mid);
    EXPECT_TRUE(mid >= lesser);
    EXPECT_FALSE(lesser >= mid);
    EXPECT_FALSE(mid >= larger);

    EXPECT_TRUE(lesser == lesser);
    EXPECT_TRUE(mid == mid);
    EXPECT_TRUE(larger == larger);

    EXPECT_FALSE(lesser != lesser);
    EXPECT_FALSE(mid != mid);
    EXPECT_FALSE(larger != larger);
}

TEST(WeakPtrTests, DefaultCtor)
{
    WeakPtr<int> ptr;
    EXPECT_EQ(ptr.GetCount(), 0);
}

TEST(WeakPtrTests, SharedPtrCtor)
{
    SharedPtr<int> null;
    SharedPtr<int> ptr = MakeShared<int>(5);

    WeakPtr<int> weakNull = null;
    WeakPtr<int> weak = ptr;

    EXPECT_EQ(weakNull.GetCount(), 0);
    EXPECT_EQ(weak.GetCount(), 1);
    EXPECT_EQ(weak.Lock(), ptr);
}

TEST(WeakPtrTests, CopyCtor)
{
    SharedPtr<int> ptr = MakeShared<int>(5);

    WeakPtr<int> weak = ptr;
    WeakPtr<int> copy = weak;

    EXPECT_EQ(copy.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 1);

    EXPECT_EQ(weak.Lock(), ptr);
    EXPECT_EQ(copy.Lock(), ptr);
}

TEST(WeakPtrTests, TemplatedCopyCtor)
{
    SharedPtr<C> ptr = MakeShared<C>();

    WeakPtr<C> weak = ptr;
    WeakPtr<B> copy = weak;

    EXPECT_EQ(copy.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 1);

    EXPECT_EQ(weak.Lock(), ptr);
    EXPECT_EQ(copy.Lock(), ptr);
}

TEST(WeakPtrTests, MoveCtor)
{
    SharedPtr<int> ptr = MakeShared<int>();

    WeakPtr<int> weak = ptr;
    WeakPtr<int> move = std::move(weak);

    EXPECT_EQ(move.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 0);

    EXPECT_EQ(move.Lock(), ptr);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, TemplatedMoveCtor)
{
    SharedPtr<C> ptr = MakeShared<C>();

    WeakPtr<C> weak = ptr;
    WeakPtr<B> move = std::move(weak);

    EXPECT_EQ(move.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 0);

    EXPECT_EQ(move.Lock(), ptr);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, SharedPtrAssign)
{
    WeakPtr<B> weak;

    {
        SharedPtr<C> ptr = MakeShared<C>();
        weak = ptr;

        EXPECT_EQ(weak.GetCount(), 1);
        EXPECT_EQ(weak.Lock(), ptr);
    }

    EXPECT_EQ(weak.GetCount(), 0);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, CopyAssign)
{
    SharedPtr<int> ptr = MakeShared<int>();
    SharedPtr<int> ptr2 = MakeShared<int>();

    WeakPtr<int> weak = ptr;
    WeakPtr<int> copy = ptr2;

    copy = weak;

    EXPECT_EQ(weak.GetCount(), 1);
    EXPECT_EQ(copy.GetCount(), 1);

    EXPECT_EQ(copy.Lock(), ptr);
}

TEST(WeakPtrTests, TemplatedCopyAssign)
{
    SharedPtr<C> ptr = MakeShared<C>();
    SharedPtr<B> ptr2 = MakeShared<B>();

    WeakPtr<C> weak = ptr;
    WeakPtr<B> copy = ptr2;

    copy = weak;

    EXPECT_EQ(weak.GetCount(), 1);
    EXPECT_EQ(copy.GetCount(), 1);

    EXPECT_EQ(copy.Lock(), ptr);
}

TEST(WeakPtrTests, MoveAssign)
{
    SharedPtr<int> ptr = MakeShared<int>();
    SharedPtr<int> ptr2 = MakeShared<int>();

    WeakPtr<int> weak = ptr;
    WeakPtr<int> move = ptr2;

    move = std::move(weak);

    EXPECT_EQ(weak.GetCount(), 0);
    EXPECT_EQ(move.GetCount(), 1);

    EXPECT_EQ(move.Lock(), ptr);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, Reset)
{
    SharedPtr<int> ptr = MakeShared<int>();
    WeakPtr<int> weak = ptr;

    weak.Reset();

    EXPECT_EQ(weak.GetCount(), 0);
}

TEST(WeakPtrTests, GetCount)
{
    SharedPtr<int> ptr = MakeShared<int>();
    WeakPtr<int> weak = ptr;

    EXPECT_EQ(weak.GetCount(), 1);

    SharedPtr<int> copy = ptr;
    EXPECT_EQ(weak.GetCount(), 2);
}

TEST(WeakPtrTests, IsExpired)
{
    WeakPtr<B> weak;
    WeakPtr<B> null;

    {
        SharedPtr<C> ptr = MakeShared<C>();
        weak = ptr;
    }

    EXPECT_TRUE(weak.IsExpired());
    EXPECT_TRUE(null.IsExpired());
}

TEST(WeakPtrTests, Lock)
{
    WeakPtr<B> weak;

    {
        SharedPtr<C> ptr = MakeShared<C>();
        weak = ptr;

        EXPECT_EQ(weak.Lock(), ptr);
    }

    EXPECT_EQ(weak.Lock(), nullptr);
}
