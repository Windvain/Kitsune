#include <gtest/gtest.h>

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/ScopedPtr.h"

namespace ScopedPtrTesting
{
    template<typename T>
    class D
    {
    public:
        using ValueType = T;

        D() = default;
        D(int value) : Value(value) { /* ... */ }

        D(const D&) = default;
        D(D&& deleter)
        {
            Value = deleter.Value;
            deleter.Value = 0;
        }

        template<typename U>
        D(const D<U>&) requires (std::is_convertible_v<U*, T*>) { /* ... */ }

        template<typename U>
        D(D<U>&& deleter)
            requires (std::is_convertible_v<U*, T*>)
        {
            Value = deleter.Value;
            deleter.Value = 0;
        }

        D& operator=(const D&) = default;

    public:
        void operator()(ValueType* ptr)
        {
            Kitsune::Memory::Delete(ptr);
        }

    public:
        int Value;
    };

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

        E(const E&) = default;
        E(E&& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            ID = deleter.ID;

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

    private:
        template<typename U>
        friend class E;

        T** m_DeletedPtr;
    };

    class B { /* ... */ };
    class C : public B
    {
    };
}

using namespace Kitsune;
using namespace ScopedPtrTesting;

TEST(ScopedPtrTests, DefaultNullptrCtor)
{
    ScopedPtr<int> ptr;
    ScopedPtr<int> null = nullptr;

    EXPECT_EQ(ptr.Get(), nullptr);
    EXPECT_EQ(null.Get(), nullptr);
}

TEST(ScopedPtrTests, PointerCtor)
{
    int* raw = Memory::New<int>(10);
    auto ptr = ScopedPtr<int>(raw);

    EXPECT_EQ(ptr.Get(), raw);
}

TEST(ScopedPtrTests, DeleterCtor)
{
    int *raw1, *raw2;
    raw1 = Memory::New<int>();
    raw2 = Memory::New<int>();

    D<int> deleter = D<int>(2);
    auto copied = ScopedPtr<int, D<int>>(raw1, deleter);
    EXPECT_EQ(copied.GetDeleter().Value, deleter.Value);
    EXPECT_EQ(copied.Get(), raw1);

    auto moved = ScopedPtr<int, D<int>>(raw2, std::move(deleter));
    EXPECT_EQ(moved.GetDeleter().Value, 2);
    EXPECT_EQ(moved.Get(), raw2);
    EXPECT_EQ(deleter.Value, 0);
}

TEST(ScopedPtrTests, MoveCtor)
{
    int* raw = Memory::New<int>();

    auto ptr = ScopedPtr<int, D<int>>(raw, D<int>(100));
    auto move = std::move(ptr);

    EXPECT_EQ(move.GetDeleter().Value, 100);
    EXPECT_EQ(ptr.GetDeleter().Value, 0);

    EXPECT_EQ(move.Get(), raw);
    EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(ScopedPtrTests, TemplatedMoveCtor)
{
    C* raw = Memory::New<C>();

    auto ptr = ScopedPtr<C, D<C>>(raw, D<C>(100));
    ScopedPtr<B, D<B>> move = std::move(ptr);

    EXPECT_EQ(move.GetDeleter().Value, 100);
    EXPECT_EQ(ptr.GetDeleter().Value, 0);

    EXPECT_EQ(move.Get(), raw);
    EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(ScopedPtrTests, Destructor)
{
    int* deletedPtr = nullptr;
    int* raw = Memory::New<int>();

    E<int> deleter = E<int>(&deletedPtr);

    {
        auto ptr = ScopedPtr<int, E<int>>(raw, deleter);
        KITSUNE_UNUSED(ptr);
    }

    EXPECT_EQ(deletedPtr, raw);
}

TEST(ScopedPtrTests, MoveAssign)
{
    int* raw = Memory::New<int>();
    int* raw2 = Memory::New<int>();

    int* moveDeleted = nullptr;

    auto ptr = ScopedPtr<int, E<int>>(raw, E<int>(nullptr, 27));
    auto move = ScopedPtr<int, E<int>>(raw2, E<int>(&moveDeleted));

    move = std::move(ptr);

    EXPECT_EQ(moveDeleted, raw2);       // Pointer deleted before being replaced.

    EXPECT_EQ(move.GetDeleter().ID, 27);
    EXPECT_EQ(ptr.GetDeleter().ID, 0);

    EXPECT_EQ(move.Get(), raw);
    EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(ScopedPtrTests, NullptrAssign)
{
    int* raw = Memory::New<int>();

    int* moveDeleted = nullptr;
    auto move = ScopedPtr<int, E<int>>(raw, E<int>(&moveDeleted, 27));

    move = nullptr;

    EXPECT_EQ(moveDeleted, raw);       // Pointer deleted before being replaced.
    EXPECT_EQ(move.Get(), nullptr);
    EXPECT_EQ(move.GetDeleter().ID, 27);
}

TEST(ScopedPtrTests, TemplatedMoveAssign)
{
    C* raw = Memory::New<C>();
    B* raw2 = Memory::New<B>();

    B* moveDeleted = nullptr;

    auto ptr = ScopedPtr<C, E<C>>(raw, E<C>(nullptr, 27));
    auto move = ScopedPtr<B, E<B>>(raw2, E<B>(&moveDeleted));

    move = std::move(ptr);

    EXPECT_EQ(moveDeleted, raw2);       // Pointer deleted before being replaced.

    EXPECT_EQ(move.GetDeleter().ID, 27);
    EXPECT_EQ(ptr.GetDeleter().ID, 0);

    EXPECT_EQ(move.Get(), raw);
    EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(ScopedPtrTests, Boolean)
{
    auto ptr = MakeScoped<int>(5);
    auto empty = ScopedPtr<int>();

    EXPECT_TRUE((bool)ptr);
    EXPECT_FALSE((bool)empty);
}

TEST(ScopedPtrTests, Release)
{
    int* x = Memory::New<int>(5);
    auto ptr = ScopedPtr<int>(x);

    EXPECT_EQ(ptr.Release(), x);
    EXPECT_EQ(ptr.Get(), nullptr);

    Memory::Delete<int>(x);
}

TEST(ScopedPtrTests, Reset)
{
    int* x = Memory::New<int>();
    int* y = Memory::New<int>();

    int* deleted = nullptr;

    auto ptr = ScopedPtr<int, E<int>>(y, E<int>(&deleted));
    ptr.Reset(x);

    EXPECT_EQ(deleted, y);
    EXPECT_EQ(ptr.Get(), x);
}

TEST(ScopedPtrTests, SwapMemberFn)
{
    ScopedPtr<int> ptr = MakeScoped<int>(5);
    ScopedPtr<int> ptr2 = MakeScoped<int>(10);

    int* rawPtr = ptr.Get();
    int* rawPtr2 = ptr2.Get();

    ptr.Swap(ptr2);

    EXPECT_EQ(ptr.Get(), rawPtr2);
    EXPECT_EQ(ptr2.Get(), rawPtr);
}

TEST(ScopedPtrTests, SwapAlgorithm)
{
    ScopedPtr<int> ptr = MakeScoped<int>(5);
    ScopedPtr<int> ptr2 = MakeScoped<int>(10);

    int* rawPtr = ptr.Get();
    int* rawPtr2 = ptr2.Get();

    Algorithms::Swap(ptr, ptr2);

    EXPECT_EQ(ptr.Get(), rawPtr2);
    EXPECT_EQ(ptr2.Get(), rawPtr);
}

TEST(ScopedPtrTests, Comparison)
{
    int* mem = (int*)Memory::Allocate(sizeof(int) * 3);
    int* null = nullptr;

    ScopedPtr<int> lesser(mem);
    ScopedPtr<int> mid(mem + 1);
    ScopedPtr<int> larger(mem + 2);

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

    // Careful..
    (void)lesser.Release();
    (void)mid.Release();
    (void)larger.Release();

    Memory::Free(mem);
}
