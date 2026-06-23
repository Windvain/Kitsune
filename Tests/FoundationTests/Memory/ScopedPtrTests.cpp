#include <algorithm>
#include <gtest/gtest.h>

#include "Foundation/Memory/ScopedPtr.h"

namespace
{
    using namespace Kitsune;

    template<typename T>
    class MyDeleter
    {
    public:
        using ValueType = T;

        MyDeleter() = default;
        MyDeleter(const MyDeleter& deleter) = default;

        inline explicit MyDeleter(int value)
            : m_Id(value)
        {
        }

        MyDeleter(MyDeleter&& deleter)
            : m_Id(std::exchange(deleter.m_Id, 0))
        {
        }

        template<typename U>
        inline MyDeleter(MyDeleter<U>&& deleter)
            : m_Id(std::exchange(deleter.m_Id, 0))
        {
        }

    public:
        MyDeleter& operator=(const MyDeleter& deleter) = default;
        inline MyDeleter& operator=(MyDeleter&& deleter)
        {
            m_Id = std::exchange(deleter.m_Id, 0);
            return *this;
        }

        template<typename U>
        inline MyDeleter& operator=(MyDeleter<U>&& deleter)
        {
            m_Id = std::exchange(deleter.m_Id, 0);
            return *this;
        }

    public:
        void operator()(T* pointer)
        {
            Memory::Delete(pointer);
        }

        [[nodiscard]]
        inline int GetId() const
        {
            return m_Id;
        }

    private:
        template<typename U>
        friend class MyDeleter;

        int m_Id;
    };

    class ExtTrackingDeleter
    {
    public:
        using ValueType = int;

    public:
        ExtTrackingDeleter() = default;
        inline ExtTrackingDeleter(const ExtTrackingDeleter&) = default;

        ExtTrackingDeleter& operator=(const ExtTrackingDeleter&) = default;

    public:
        inline void operator()(ValueType* pointer)
        {
            m_Deletions.push_back(pointer);
            Memory::Delete(pointer);
        }

    public:
        inline static void Reset()
        {
            m_Deletions.clear();
        }

        [[nodiscard]]
        inline static bool IsDeleted(int* pointer)
        {
            return (std::find(
                m_Deletions.begin(),
                m_Deletions.end(),
                pointer) != m_Deletions.end());
        }

    private:
        static std::vector<int*> m_Deletions;
    };

    std::vector<int*> ExtTrackingDeleter::m_Deletions;

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

    class ScopedPtrTest : public ::testing::Test
    {
    public:
        inline void TearDown() override
        {
            ExtTrackingDeleter::Reset();
        }
    };

    TEST_F(ScopedPtrTest, DefaultConstructor)
    {
        ScopedPtr<int> pointer;
        EXPECT_EQ(pointer.Get(), nullptr);
    }

    TEST_F(ScopedPtrTest, NullptrConstructor)
    {
        ScopedPtr<int> pointer = nullptr;
        EXPECT_EQ(pointer.Get(), nullptr);
    }

    TEST_F(ScopedPtrTest, PointerConstructor)
    {
        int* rawPointer = Memory::New<int>(345);
        ScopedPtr<int> pointer(rawPointer);

        EXPECT_EQ(pointer.Get(), rawPointer);
    }

    TEST_F(ScopedPtrTest, PointerDeleterConstructor1)
    {
        int* rawPointer = Memory::New<int>(345);
        ScopedPtr<int, MyDeleter<int>> pointer(rawPointer, MyDeleter<int>(23));

        EXPECT_EQ(pointer.Get(), rawPointer);
        EXPECT_EQ(pointer.GetDeleter().GetId(), 23);
    }

    TEST_F(ScopedPtrTest, PointerDeleterConstructor2)
    {
        int* rawPointer = Memory::New<int>(345);
        MyDeleter<int> deleter(23);

        ScopedPtr<int, MyDeleter<int>> pointer(rawPointer, deleter);

        EXPECT_EQ(pointer.Get(), rawPointer);
        EXPECT_EQ(pointer.GetDeleter().GetId(), 23);
    }

    TEST_F(ScopedPtrTest, MoveConstructor)
    {
        int* rawPointer = Memory::New<int>(345);

        ScopedPtr<int, MyDeleter<int>> pointer(rawPointer, MyDeleter<int>(234));
        ScopedPtr<int, MyDeleter<int>> moved = Move(pointer);

        EXPECT_EQ(pointer.Get(), nullptr);
        EXPECT_EQ(pointer.GetDeleter().GetId(), 0);

        EXPECT_EQ(moved.Get(), rawPointer);
        EXPECT_EQ(moved.GetDeleter().GetId(), 234);
    }

    TEST_F(ScopedPtrTest, TemplatedMoveConstructor)
    {
        auto* rawPointer = Memory::New<Derived>(345);
        ScopedPtr<Derived, MyDeleter<Derived>> pointer(
            rawPointer, MyDeleter<Derived>(234));

        ScopedPtr<Base, MyDeleter<Base>> moved = Move(pointer);

        EXPECT_EQ(pointer.Get(), nullptr);
        EXPECT_EQ(pointer.GetDeleter().GetId(), 0);

        EXPECT_EQ(moved.Get(), rawPointer);
        EXPECT_EQ(moved.GetDeleter().GetId(), 234);
    }

    TEST_F(ScopedPtrTest, Destructor)
    {
        int* rawPointer = Memory::New<int>(2345);

        {
            ScopedPtr<int, ExtTrackingDeleter> pointer(rawPointer);
            KITSUNE_UNUSED(pointer);
        }

        EXPECT_TRUE(ExtTrackingDeleter::IsDeleted(rawPointer));
    }

    TEST_F(ScopedPtrTest, MoveAssign)
    {
        int* rawPointer = Memory::New<int>(345);
        int* secondPointer = Memory::New<int>(452);

        ScopedPtr<int, ExtTrackingDeleter> pointer(rawPointer);
        ScopedPtr<int, ExtTrackingDeleter> moved(secondPointer);

        moved = Move(pointer);

        EXPECT_EQ(pointer.Get(), nullptr);
        EXPECT_EQ(moved.Get(), rawPointer);

        EXPECT_TRUE(ExtTrackingDeleter::IsDeleted(secondPointer));
    }

    TEST_F(ScopedPtrTest, MoveAssignMovesDeleter)
    {
        int* rawPointer = Memory::New<int>(345);
        int* secondPointer = Memory::New<int>(452);

        ScopedPtr<int, MyDeleter<int>> pointer(rawPointer, MyDeleter<int>(23));
        ScopedPtr<int, MyDeleter<int>> moved(secondPointer, MyDeleter<int>(4));

        moved = Move(pointer);

        EXPECT_EQ(pointer.GetDeleter().GetId(), 0);
        EXPECT_EQ(moved.GetDeleter().GetId(), 23);
    }

    TEST_F(ScopedPtrTest, NullptrAssign)
    {
        int* rawPointer = Memory::New<int>(345);

        ScopedPtr<int, ExtTrackingDeleter> pointer(rawPointer);
        pointer = nullptr;

        EXPECT_TRUE(ExtTrackingDeleter::IsDeleted(rawPointer));
        EXPECT_EQ(pointer.Get(), nullptr);
    }

    TEST_F(ScopedPtrTest, TemplatedMoveAssign)
    {
        auto* rawPointer = Memory::New<Derived>(345);
        auto* secondPointer = Memory::New<Derived>(452);

        ScopedPtr<Derived, MyDeleter<Derived>> pointer(
            rawPointer, MyDeleter<Derived>(34));

        ScopedPtr<Base, MyDeleter<Base>> moved(secondPointer, MyDeleter<Base>(2341));
        moved = Move(pointer);

        EXPECT_EQ(pointer.Get(), nullptr);
        EXPECT_EQ(pointer.GetDeleter().GetId(), 0);

        EXPECT_EQ(moved.Get(), rawPointer);
        EXPECT_EQ(moved.GetDeleter().GetId(), 34);
    }

    /* Unable to test whether ScopedPtr<T>::operator=(ScopedPtr<U>&&) deletes the
     * pointer.
     */

    TEST_F(ScopedPtrTest, Dereference)
    {
        int* rawPointer = Memory::New<int>(10);
        ScopedPtr<int> pointer(rawPointer);

        EXPECT_EQ(std::addressof(*pointer), rawPointer);
    }

    TEST_F(ScopedPtrTest, ArrowOperator)
    {
        int* rawPointer = Memory::New<int>(10);
        ScopedPtr<int> pointer(rawPointer);

        EXPECT_EQ(pointer.operator->(), rawPointer);
    }

    TEST_F(ScopedPtrTest, Boolean)
    {
        ScopedPtr<int> pointer(Memory::New<int>(10));
        ScopedPtr<int> null;

        EXPECT_TRUE(pointer);
        EXPECT_FALSE(null);
    }

    TEST_F(ScopedPtrTest, Release)
    {
        int* rawPointer = Memory::New<int>(23451);
        ScopedPtr<int> pointer(rawPointer);

        EXPECT_EQ(pointer.Release(), rawPointer);
        EXPECT_EQ(pointer.Get(), nullptr);

        Memory::Delete(rawPointer);
    }

    TEST_F(ScopedPtrTest, Reset)
    {
        int* rawPointer = Memory::New<int>(34567);
        ScopedPtr<int, ExtTrackingDeleter> pointer(rawPointer);

        int* secondPointer = Memory::New<int>(5489);
        pointer.Reset(secondPointer);

        EXPECT_EQ(pointer.Get(), secondPointer);
        EXPECT_TRUE(ExtTrackingDeleter::IsDeleted(rawPointer));

        pointer.Reset();

        EXPECT_EQ(pointer.Get(), nullptr);
        EXPECT_TRUE(ExtTrackingDeleter::IsDeleted(secondPointer));
    }

    TEST_F(ScopedPtrTest, Swap)
    {
        int* firstPointer = Memory::New<int>(345);
        int* secondPointer = Memory::New<int>(3491);

        ScopedPtr<int, MyDeleter<int>> pointer1(firstPointer, MyDeleter<int>(345231));
        ScopedPtr<int, MyDeleter<int>> pointer2(secondPointer, MyDeleter<int>(423));

        pointer1.Swap(pointer2);

        EXPECT_EQ(pointer1.Get(), secondPointer);
        EXPECT_EQ(pointer1.GetDeleter().GetId(), 423);

        EXPECT_EQ(pointer2.Get(), firstPointer);
        EXPECT_EQ(pointer2.GetDeleter().GetId(), 345231);
    }

    /* Get() and GetDeleter() are basic functions, not testing.. */

    TEST_F(ScopedPtrTest, MakeScoped)
    {
        auto pointer = MakeScoped<int>(345);
        EXPECT_NE(pointer.Get(), nullptr);
        EXPECT_EQ(*pointer.Get(), 345);
    }

    TEST_F(ScopedPtrTest, Comparison)
    {
        int* buffer = static_cast<int*>(Memory::Allocate(sizeof(int) * 3));
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

        EXPECT_TRUE(lesser < mid.Get());
        EXPECT_TRUE(mid < larger.Get());
        EXPECT_FALSE(larger < mid.Get());
        EXPECT_FALSE(mid < lesser.Get());

        EXPECT_TRUE(larger > mid.Get());
        EXPECT_TRUE(mid > lesser.Get());
        EXPECT_FALSE(mid > larger.Get());
        EXPECT_FALSE(lesser > mid.Get());

        EXPECT_TRUE(lesser <= mid.Get());
        EXPECT_TRUE(mid <= mid.Get());
        EXPECT_TRUE(mid <= larger.Get());
        EXPECT_FALSE(mid <= lesser.Get());
        EXPECT_FALSE(larger <= mid.Get());

        EXPECT_TRUE(larger >= mid.Get());
        EXPECT_TRUE(mid >= mid.Get());
        EXPECT_TRUE(mid >= lesser.Get());
        EXPECT_FALSE(lesser >= mid.Get());
        EXPECT_FALSE(mid >= larger.Get());

        EXPECT_TRUE(lesser == lesser.Get());
        EXPECT_TRUE(mid == mid.Get());
        EXPECT_TRUE(larger == larger.Get());

        EXPECT_FALSE(lesser != lesser.Get());
        EXPECT_FALSE(mid != mid.Get());
        EXPECT_FALSE(larger != larger.Get());

        EXPECT_TRUE(lesser.Get() < mid);
        EXPECT_TRUE(mid.Get() < larger);
        EXPECT_FALSE(larger.Get() < mid);
        EXPECT_FALSE(mid.Get() < lesser);

        EXPECT_TRUE(larger.Get() > mid);
        EXPECT_TRUE(mid.Get() > lesser);
        EXPECT_FALSE(mid.Get() > larger);
        EXPECT_FALSE(lesser.Get() > mid);

        EXPECT_TRUE(lesser.Get() <= mid);
        EXPECT_TRUE(mid.Get() <= mid);
        EXPECT_TRUE(mid.Get() <= larger);
        EXPECT_FALSE(mid.Get() <= lesser);
        EXPECT_FALSE(larger.Get() <= mid);

        EXPECT_TRUE(larger.Get() >= mid);
        EXPECT_TRUE(mid.Get() >= mid);
        EXPECT_TRUE(mid.Get() >= lesser);
        EXPECT_FALSE(lesser.Get() >= mid);
        EXPECT_FALSE(mid.Get() >= larger);

        EXPECT_TRUE(lesser.Get() == lesser);
        EXPECT_TRUE(mid.Get() == mid);
        EXPECT_TRUE(larger.Get() == larger);

        EXPECT_FALSE(lesser.Get() != lesser);
        EXPECT_FALSE(mid.Get() != mid);
        EXPECT_FALSE(larger.Get() != larger);

        (void)lesser.Release();
        (void)mid.Release();
        (void)larger.Release();

        Memory::Free(buffer, sizeof(int) * 3);
    }
}
