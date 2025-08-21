#include <gtest/gtest.h>

#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/ScopedPtr.h"

using namespace Kitsune;

namespace
{
    template<typename T>
    class D
    {
    public:
        using ValueType = T;

        D() = default;
        D(int value)
            : Value(value)
        {
        }

        D(D&& deleter)
        {
            Value = deleter.Value;
            deleter.Value = 0;
        }

        D(const D&) = default;
        D& operator=(const D&) = default;

        template<typename U>
        D(const D<U>&)
            requires (std::is_convertible_v<U*, T*>)
        {
        }

        template<typename U>
        D(D<U>&& deleter)
            requires (std::is_convertible_v<U*, T*>)
        {
            Value = deleter.Value;
            deleter.Value = 0;
        }

    public:
        void operator()(ValueType* ptr)
        {
            Memory::Delete(ptr);
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
            : Id(id), m_DeletedPtr(deletedPtr)
        {
        }

        E(const E&) = default;
        E(E&& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            deleter.m_DeletedPtr = nullptr;
            deleter.Id = 0;
        }

        ~E() = default;

    public:
        E& operator=(const E& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            return *this;
        }

        template<typename U>
        E& operator=(const E<U>& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            return *this;
        }

        E& operator=(E&& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            deleter.m_DeletedPtr = nullptr;
            deleter.Id = 0;

            return *this;
        }

        template<typename U>
        E& operator=(E<U>&& deleter)
        {
            m_DeletedPtr = (T**)deleter.m_DeletedPtr;
            Id = deleter.Id;

            deleter.m_DeletedPtr = nullptr;
            deleter.Id = 0;

            return *this;
        }

        void operator()(T* ptr)
        {
            if (m_DeletedPtr)
                *m_DeletedPtr = ptr;

            Memory::Delete(ptr);
        }

    public:
        int Id = 0;

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

TEST(ScopedPtrTests, DefaultNullptrConstructor)
{
    ScopedPtr<int> ptr;
    ScopedPtr<int> null = nullptr;

    EXPECT_EQ(ptr.Get(), nullptr);
    EXPECT_EQ(null.Get(), nullptr);
}

TEST(ScopedPtrTests, PointerConstructor)
{
    int* raw = Memory::New<int>(10);
    auto ptr = ScopedPtr<int>(raw);

    EXPECT_EQ(ptr.Get(), raw);
}

TEST(ScopedPtrTests, DeleterCopyConstructor)
{
    int* rawPointer = Memory::New<int>();

    D<int> deleter(24);
    ScopedPtr<int, D<int>> ptr(rawPointer, deleter);

    EXPECT_EQ(ptr.Get(), rawPointer);
    EXPECT_EQ(ptr.GetDeleter().Value, 24);
}

TEST(ScopedPtrTests, DeleterMoveConstructor)
{
    int* rawPointer = Memory::New<int>();

    D<int> deleter(24);
    ScopedPtr<int, D<int>> ptr(rawPointer, std::move(deleter));

    EXPECT_EQ(ptr.Get(), rawPointer);

    EXPECT_EQ(ptr.GetDeleter().Value, 24);
    EXPECT_EQ(deleter.Value, 0);
}

TEST(ScopedPtrTests, MoveConstructor)
{
    int* raw = Memory::New<int>();

    ScopedPtr<int, D<int>> ptr(raw, D<int>(100));
    auto move = std::move(ptr);

    EXPECT_EQ(move.GetDeleter().Value, 100);
    EXPECT_EQ(move.Get(), raw);

    EXPECT_EQ(ptr.GetDeleter().Value, 0);
    EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(ScopedPtrTests, TemplatedMoveConstructor)
{
    C* raw = Memory::New<C>();

    ScopedPtr<C, D<C>> ptr(raw, D<C>(100));
    ScopedPtr<B, D<B>> move = std::move(ptr);

    EXPECT_EQ(move.GetDeleter().Value, 100);
    EXPECT_EQ(move.Get(), raw);

    EXPECT_EQ(ptr.GetDeleter().Value, 0);
    EXPECT_EQ(ptr.Get(), nullptr);
}

TEST(ScopedPtrTests, Destructor)
{
    int* raw = Memory::New<int>();

    int* deletedPtr = nullptr;
    E<int> deleter(&deletedPtr);

    {
        ScopedPtr<int, E<int>> ptr(raw, deleter);
        ASSERT_EQ(ptr.Get(), raw);
        ASSERT_EQ(ptr.GetDeleter().Id, deleter.Id);
    }

    EXPECT_EQ(deletedPtr, raw);
}

TEST(ScopedPtrTests, MoveAssign)
{
    int* rawPointer = Memory::New<int>();
    int* deletedRawPointer = Memory::New<int>();

    int* deletedPointer = nullptr;

    ScopedPtr<int, E<int>> ptr(deletedRawPointer, E<int>(&deletedPointer));
    ScopedPtr<int, E<int>> movedPointer(rawPointer, E<int>(nullptr, 27));

    ptr = std::move(movedPointer);

    EXPECT_EQ(deletedPointer, deletedRawPointer);

    EXPECT_EQ(ptr.Get(), rawPointer);
    EXPECT_EQ(ptr.GetDeleter().Id, 27);
}

TEST(ScopedPtrTests, NullptrAssign)
{
    int* raw = Memory::New<int>();

    int* moveDeleted = nullptr;
    auto move = ScopedPtr<int, E<int>>(raw, E<int>(&moveDeleted, 27));

    move = nullptr;

    EXPECT_EQ(moveDeleted, raw);

    EXPECT_EQ(move.Get(), nullptr);
    EXPECT_EQ(move.GetDeleter().Id, 27);
}

TEST(ScopedPtrTests, TemplatedMoveAssign)
{
    C* rawPointer = Memory::New<C>();
    B* deletedRawPointer = Memory::New<B>();

    B* deletedPointer = nullptr;

    ScopedPtr<B, E<B>> ptr(deletedRawPointer, E<B>(&deletedPointer));
    ScopedPtr<C, E<C>> movedPointer(rawPointer, E<C>(nullptr, 27));

    ptr = std::move(movedPointer);

    EXPECT_EQ(deletedPointer, deletedRawPointer);

    EXPECT_EQ(ptr.Get(), rawPointer);
    EXPECT_EQ(ptr.GetDeleter().Id, 27);
}

TEST(ScopedPtrTests, Boolean)
{
    ScopedPtr<int> ptr(Memory::New<int>());
    ScopedPtr<int> empty{ /* ... */ };

    EXPECT_TRUE((bool)ptr);
    EXPECT_FALSE((bool)empty);
}

TEST(ScopedPtrTests, Release)
{
    int* rawPointer = Memory::New<int>(5);
    ScopedPtr<int> ptr(rawPointer);

    EXPECT_EQ(ptr.Release(), rawPointer);
    EXPECT_EQ(ptr.Get(), nullptr);

    Memory::Delete<int>(rawPointer);
}

TEST(ScopedPtrTests, Reset)
{
    int* x = Memory::New<int>();
    int* y = Memory::New<int>();

    int* deleted = nullptr;

    ScopedPtr<int, E<int>> ptr(y, E<int>(&deleted));
    ptr.Reset(x);

    EXPECT_EQ(deleted, y);
    EXPECT_EQ(ptr.Get(), x);
}

TEST(ScopedPtrTests, SwapMemberFunction)
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
