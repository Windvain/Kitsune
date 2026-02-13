#include "Foundation/Memory/ScopedPtr.h"
#include <gtest/gtest.h>

using namespace Kitsune;

namespace
{
    template<typename T>
    class MyDeleter
    {
    public:
        using ValueType = T;

        MyDeleter() = default;
        explicit MyDeleter(int value)
            : Value(value)
        {
        }

        MyDeleter(const MyDeleter& deleter)
            : Value(deleter.Value)
        {
        }

        MyDeleter(MyDeleter&& deleter)
            : Value(deleter.Value)
        {
            deleter.Value = 0;
        }

        template<std::convertible_to<T> U>
        MyDeleter(MyDeleter<U>&& deleter)
            : Value(deleter.Value)
        {
            deleter.Value = 0;
        }

        MyDeleter& operator=(const MyDeleter& deleter)
        {
            Value = deleter.Value;
            return *this;
        }

        void operator()(T* pointer)
        {
            Memory::Delete(pointer);
        }

    public:
        int Value;
    };

    template<typename T>
    class NotingDeleter
    {
    public:
        using ValueType = T;

        NotingDeleter() = default;
        NotingDeleter(T** deletedPointer, int Id = 0)
            : m_Deleted(deletedPointer), m_Id(Id)
        {
        }

        NotingDeleter(const NotingDeleter&) = default;
        NotingDeleter(NotingDeleter&& deleter)
        {
            m_Deleted = std::exchange(deleter.m_Deleted, nullptr);
            m_Id = std::exchange(deleter.m_Id, 0);
        }

        NotingDeleter& operator=(const NotingDeleter& deleter)
        {
            m_Deleted = deleter;
            m_Id = deleter.m_Id;

            return *this;
        }

        NotingDeleter& operator=(NotingDeleter&& deleter)
        {
            m_Deleted = std::exchange(deleter.m_Deleted, nullptr);
            m_Id = std::exchange(deleter.m_Id, 0);

            return *this;
        }

        template<std::convertible_to<T> U>
        NotingDeleter& operator=(NotingDeleter<U>&& deleter)
        {
            m_Deleted = (T**)std::exchange(deleter.m_Deleted, nullptr);
            m_Id = std::exchange(deleter.m_Id, 0);

            return *this;
        }

    public:
        void operator()(T* pointer)
        {
            if (m_Deleted)
                *m_Deleted = pointer;

            Memory::Delete(pointer);
        }

        int GetId() const { return m_Id; }

    private:
        template<typename U>
        friend class NotingDeleter;

        T** m_Deleted = nullptr;
        int m_Id;
    };

    struct Base
    {
        explicit Base(int value)
            : Value(value)
        {
        }

        int Value;
    };

    struct Derived : public Base
    {
        explicit Derived(int value)
            : Base(value)
        {
        }
    };
}

TEST(ScopedPtrTests, DefaultConstructor)
{
    ScopedPtr<int> pointer;
    EXPECT_EQ(pointer.Get(), nullptr);
}

TEST(ScopedPtrTests, NullptrConstructor)
{
    ScopedPtr<int> pointer = nullptr;
    EXPECT_EQ(pointer.Get(), nullptr);
}

TEST(ScopedPtrTests, PointerConstructor)
{
    int* rawPointer = Memory::New<int>(345);
    ScopedPtr<int> pointer(rawPointer);

    EXPECT_EQ(pointer.Get(), rawPointer);
}

TEST(ScopedPtrTests, PointerDeleterConstructor1)
{
    int* rawPointer = Memory::New<int>(345);
    ScopedPtr<int, MyDeleter<int>> pointer(rawPointer, MyDeleter<int>(23));

    EXPECT_EQ(pointer.Get(), rawPointer);
    EXPECT_EQ(pointer.GetDeleter().Value, 23);
}

TEST(ScopedPtrTests, PointerDeleterConstructor2)
{
    int* rawPointer = Memory::New<int>(345);
    MyDeleter<int> deleter = MyDeleter<int>(23);

    ScopedPtr<int, MyDeleter<int>> pointer(rawPointer, deleter);

    EXPECT_EQ(pointer.Get(), rawPointer);
    EXPECT_EQ(pointer.GetDeleter().Value, 23);
}

TEST(ScopedPtrTests, MoveConstructor)
{
    int* rawPointer = Memory::New<int>(345);

    ScopedPtr<int, MyDeleter<int>> pointer(rawPointer, MyDeleter<int>(234));
    ScopedPtr<int, MyDeleter<int>> moved = Move(pointer);

    EXPECT_EQ(pointer.Get(), nullptr);
    EXPECT_EQ(pointer.GetDeleter().Value, 0);

    EXPECT_EQ(moved.Get(), rawPointer);
    EXPECT_EQ(moved.GetDeleter().Value, 234);
}

TEST(ScopedPtrTests, TemplatedMoveConstructor)
{
    Derived* rawPointer = Memory::New<Derived>(345);

    ScopedPtr<Derived, MyDeleter<Derived>> pointer(
        rawPointer, MyDeleter<Derived>(234));

    ScopedPtr<Base, MyDeleter<Base>> moved = Move(pointer);

    EXPECT_EQ(pointer.Get(), nullptr);
    EXPECT_EQ(pointer.GetDeleter().Value, 0);

    EXPECT_EQ(moved.Get(), rawPointer);
    EXPECT_EQ(moved.GetDeleter().Value, 234);
}

TEST(ScopedPtrTests, Destructor)
{
    int* rawPointer = Memory::New<int>(2345);
    int* deletedPointer = nullptr;

    {
        ScopedPtr<int, NotingDeleter<int>> pointer(
            rawPointer, NotingDeleter<int>(&deletedPointer));

        KITSUNE_UNUSED(pointer);
    }

    EXPECT_EQ(deletedPointer, rawPointer);
}

TEST(ScopedPtrTests, MoveAssign)
{
    int* rawPointer = Memory::New<int>(345);
    int* secondPointer = Memory::New<int>(452);

    int* deletedPointer = nullptr;

    ScopedPtr<int, NotingDeleter<int>> pointer(
        rawPointer, NotingDeleter<int>(nullptr, 3));

    ScopedPtr<int, NotingDeleter<int>> moved(
        secondPointer, NotingDeleter<int>(&deletedPointer, 2123));

    moved = Move(pointer);

    EXPECT_EQ(pointer.Get(), nullptr);
    EXPECT_EQ(pointer.GetDeleter().GetId(), 0);

    EXPECT_EQ(deletedPointer, secondPointer);

    EXPECT_EQ(moved.Get(), rawPointer);
    EXPECT_EQ(moved.GetDeleter().GetId(), 3);
}

TEST(ScopedPtrTests, NullptrAssign)
{
    int* rawPointer = Memory::New<int>(345);
    int* deletedPointer = nullptr;

    ScopedPtr<int, NotingDeleter<int>> pointer(
        rawPointer, NotingDeleter<int>(&deletedPointer, 3));

    pointer = nullptr;

    EXPECT_EQ(deletedPointer, rawPointer);

    EXPECT_EQ(pointer.Get(), nullptr);
    EXPECT_EQ(pointer.GetDeleter().GetId(), 3);
}

TEST(ScopedPtrTests, TemplatedMoveAssign)
{
    Derived* rawPointer = Memory::New<Derived>(345);
    Base* secondPointer = Memory::New<Derived>(452);

    Base* deletedPointer = nullptr;

    ScopedPtr<Derived, NotingDeleter<Derived>> pointer(
        rawPointer, NotingDeleter<Derived>(nullptr, 3));

    ScopedPtr<Base, NotingDeleter<Base>> moved(
        secondPointer, NotingDeleter<Base>(&deletedPointer, 2123));

    moved = Move(pointer);

    EXPECT_EQ(pointer.Get(), nullptr);
    EXPECT_EQ(pointer.GetDeleter().GetId(), 0);

    EXPECT_EQ(deletedPointer, secondPointer);

    EXPECT_EQ(moved.Get(), rawPointer);
    EXPECT_EQ(moved.GetDeleter().GetId(), 3);
}

TEST(ScopedPtrTests, Dereference)
{
    int* rawPointer = Memory::New<int>(10);
    ScopedPtr<int> pointer(rawPointer);

    EXPECT_EQ(std::addressof(*pointer), rawPointer);
}

TEST(ScopedPtrTests, ArrowOperator)
{
    int* rawPointer = Memory::New<int>(10);
    ScopedPtr<int> pointer(rawPointer);

    EXPECT_EQ(pointer.operator->(), rawPointer);

}

TEST(ScopedPtrTests, Boolean)
{
    ScopedPtr<int> pointer(Memory::New<int>(10));
    ScopedPtr<int> null;

    EXPECT_TRUE(pointer);
    EXPECT_FALSE(null);
}

TEST(ScopedPtrTests, Release)
{
    int* rawPointer = Memory::New<int>(23451);
    ScopedPtr<int> pointer(rawPointer);

    EXPECT_EQ(pointer.Release(), rawPointer);
    EXPECT_EQ(pointer.Get(), nullptr);
}

TEST(ScopedPtrTests, Reset)
{
    int* rawPointer = Memory::New<int>(34567);
    int* deletedPointer = nullptr;

    ScopedPtr<int, NotingDeleter<int>> pointer(
        rawPointer, NotingDeleter<int>(&deletedPointer));;

    int* secondPointer = Memory::New<int>(5489);
    pointer.Reset(secondPointer);

    EXPECT_EQ(deletedPointer, rawPointer);
    EXPECT_EQ(pointer.Get(), secondPointer);

    pointer.Reset();

    EXPECT_EQ(deletedPointer, secondPointer);
    EXPECT_EQ(pointer.Get(), nullptr);
}

TEST(ScopedPtrTests, Swap)
{
    int* firstPointer = Memory::New<int>(345);
    int* secondPointer = Memory::New<int>(3491);

    ScopedPtr<int, MyDeleter<int>> pointer1(
        firstPointer, MyDeleter<int>(345231));

    ScopedPtr<int, MyDeleter<int>> pointer2(
        secondPointer, MyDeleter<int>(423));

    pointer1.Swap(pointer2);

    EXPECT_EQ(pointer1.Get(), secondPointer);
    EXPECT_EQ(pointer1.GetDeleter().Value, 423);

    EXPECT_EQ(pointer2.Get(), firstPointer);
    EXPECT_EQ(pointer2.GetDeleter().Value, 345231);
}

/* Get() and GetDeleter() are basic functions, not testing.. */

TEST(ScopedPtrTests, MakeScoped)
{
    auto pointer = MakeScoped<int>(345);
    EXPECT_NE(pointer.Get(), nullptr);
    EXPECT_EQ(*pointer.Get(), 345);
}

TEST(ScopedPtrTests, Comparison)
{
    int* buffer = (int*)Memory::Allocate(sizeof(int) * 3);
    int* null = nullptr;

    ScopedPtr<int> lesser(buffer);
    ScopedPtr<int> mid(buffer + 1);
    ScopedPtr<int> larger(buffer + 2);

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

    (void)lesser.Release();
    (void)mid.Release();
    (void)larger.Release();

    Memory::Free(buffer, sizeof(int) * 3);
}
