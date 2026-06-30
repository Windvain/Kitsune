#include <gtest/gtest.h>
#include "TrackingAllocator.h"

#include "Foundation/Memory/SharedPtr.h"

namespace
{
    using namespace Kitsune;
    using Testing::TrackingAllocator;

    template<typename T>
    class TrackingDeleter
    {
    public:
        using ValueType = T;
        using StorageType = std::vector<void*>;

        inline TrackingDeleter() = default;
        inline TrackingDeleter(StorageType* storage)
            : m_Storage(storage)
        {
        }

        inline TrackingDeleter(const TrackingDeleter&) = default;
        inline TrackingDeleter(TrackingDeleter&& deleter)
            : m_Storage(std::exchange(deleter.m_Storage, nullptr))
        {
        }

        ~TrackingDeleter() = default;

    public:
        inline TrackingDeleter& operator=(const TrackingDeleter& deleter)
        {
            if (this == &deleter)
                return *this;

            KITSUNE_UNREACHABLE();
            return *this;
        }

        inline TrackingDeleter& operator=(TrackingDeleter&& deleter)
        {
            if (this == &deleter)
                return *this;

            KITSUNE_UNREACHABLE();
            return *this;
        }

    public:
        inline void operator()(T* pointer)
        {
            if ((m_Storage == nullptr) || (pointer == nullptr))
                return;

            m_Storage->push_back(pointer);
            Memory::Delete(pointer);
        }

    private:
        template<typename U>
        friend class MyDeleter;

        StorageType* m_Storage = nullptr;
    };

    static_assert(
        Deleter<TrackingDeleter<int>>,
        "TrackingDeleter<T> does not fulfill the requirements for Deleter.");

    class Base
    {
    public:
        virtual ~Base() = default;
    };

    class Derived : public Base
    {
    };

    TEST(SharedPtrTest, DefaultNullptrConstructor)
    {
        SharedPtr<int> pointer;
        SharedPtr<int> null = nullptr;

        EXPECT_EQ(pointer.Get(), nullptr);
        EXPECT_EQ(null.Get(), nullptr);

        EXPECT_EQ(pointer.GetCount(), 0);
        EXPECT_EQ(null.GetCount(), 0);
    }

    TEST(SharedPtrTest, PointerConstructor)
    {
        int* rawPointer = Memory::New<int>(234);
        auto pointer = SharedPtr<int>(rawPointer);

        EXPECT_EQ(pointer.Get(), rawPointer);
        EXPECT_EQ(pointer.GetCount(), 1);

        // rawPointer here should've been destroyed by GlobalAllocator.
    }

    TEST(SharedPtrTest, PointerDeleterConstructor)
    {
        int* rawPointer1 = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);

            SharedPtr<int> moved(rawPointer1, TrackingDeleter<int>(&deleted));
            SharedPtr<int> copied(rawPointer2, deleter);

            EXPECT_EQ(moved.Get(), rawPointer1);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(copied.Get(), rawPointer2);
            EXPECT_EQ(copied.GetCount(), 1);
        }

        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer1);
    }

    TEST(SharedPtrTest, NullptrDeleterConstructor)
    {
        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);

            SharedPtr<int> moved(nullptr, TrackingDeleter<int>(&deleted));
            SharedPtr<int> copied(nullptr, deleter);

            EXPECT_EQ(moved.Get(), nullptr);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(copied.Get(), nullptr);
            EXPECT_EQ(copied.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TEST(SharedPtrTest, CopyDeleterAndAllocatorConstructor)
    {
        int* rawPointer = Memory::New<int>();
        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);
            SharedPtr<int> pointer(rawPointer, deleter, TrackingAllocator());

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, MoveDeleterAndAllocatorConstructor)
    {
        int* rawPointer = Memory::New<int>();
        std::vector<void*> deleted;

        {
            SharedPtr<int> pointer(
                rawPointer,
                TrackingDeleter<int>(&deleted),
                TrackingAllocator());

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, NullptrCopyDeleterAndAllocatorConstructor)
    {
        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);
            SharedPtr<int> pointer(nullptr, deleter, TrackingAllocator());

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TEST(SharedPtrTest, NullptrMoveDeleterAndAllocatorConstructor)
    {
        std::vector<void*> deleted;

        {
            SharedPtr<int> pointer(
                nullptr,
                TrackingDeleter<int>(&deleted),
                TrackingAllocator());

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TEST(SharedPtrTest, CopyConstructor)
    {
        int* rawPointer = Memory::New<int>(4);
        std::vector<void*> deleted;

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            SharedPtr<int> copy = pointer;

            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, AliasingCopyConstructor)
    {
        int* rawPointer = Memory::New<int>(45);
        std::vector<void*> deleted;

        {
            SharedPtr<int> ownedPointer(rawPointer, TrackingDeleter<int>(&deleted));

            int* storedPointer = reinterpret_cast<int*>(0xDEADC0DE);
            SharedPtr<int> pointer(ownedPointer, storedPointer);

            EXPECT_EQ(pointer.Get(), storedPointer);
            EXPECT_EQ(pointer.GetCount(), 2);

            EXPECT_EQ(ownedPointer.Get(), rawPointer);
            EXPECT_EQ(ownedPointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, TemplatedCopyConstructor)
    {
        auto* rawPointer = Memory::New<Derived>();
        std::vector<void*> deleted;

        {
            SharedPtr<Derived> pointer(rawPointer, TrackingDeleter<Derived>(&deleted));

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            SharedPtr<Base> copy = pointer;

            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, TemplatedCopyConstructorHandlesNullptr)
    {
        std::vector<void*> deleted;

        {
            SharedPtr<Derived> pointer(nullptr, TrackingDeleter<Derived>(&deleted));

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            SharedPtr<Base> copy = pointer;

            EXPECT_EQ(copy.Get(), nullptr);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TEST(SharedPtrTest, MoveConstructor)
    {
        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>(5);

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
            SharedPtr<int> moved = std::move(pointer);

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, TemplatedMoveConstructor)
    {
        std::vector<void*> deleted;
        auto* rawPointer = Memory::New<Derived>();

        {
            SharedPtr<Derived> pointer(rawPointer, TrackingDeleter<Derived>(&deleted));
            SharedPtr<Base> moved(std::move(pointer));

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, AliasingMoveConstructor)
    {
        int* rawPointer = Memory::New<int>(45);
        std::vector<void*> deleted;

        {
            SharedPtr<int> ownedPointer(rawPointer, TrackingDeleter<int>(&deleted));

            int* storedPointer = reinterpret_cast<int*>(0xDEADC0DE);
            SharedPtr<int> pointer(std::move(ownedPointer), storedPointer);

            EXPECT_EQ(pointer.Get(), storedPointer);
            EXPECT_EQ(pointer.GetCount(), 1);

            EXPECT_EQ(ownedPointer.Get(), nullptr);
            EXPECT_EQ(ownedPointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, WeakPtrConstructor)
    {
        int* rawPointer = Memory::New<int>(45);
        std::vector<void*> deleted;

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
            WeakPtr<int> weakPointer(pointer);

            ASSERT_EQ(pointer.GetCount(), 1);
            ASSERT_EQ(pointer.Get(), rawPointer);

            SharedPtr<int> copy(weakPointer);
            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);

        WeakPtr<int> weakNull;
        ASSERT_TRUE(weakNull.IsExpired());

        EXPECT_THROW(
            SharedPtr<int> pointer(weakNull),
            BadWeakPtrException);
    }

    TEST(SharedPtrTest, ScopedPtrConstructor)
    {
        auto* rawPointer = Memory::New<Derived>();
        std::vector<void*> deleted;

        {
            ScopedPtr<Derived, TrackingDeleter<Derived>> pointer(
                rawPointer,
                TrackingDeleter<Derived>(&deleted));

            SharedPtr<Base> moved(std::move(pointer));
            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, Destructor)
    {
        int* rawPointer = Memory::New<int>(5);
        std::vector<void*> deleted;

        {
            SharedPtr<int> pointer = SharedPtr<int>(
                rawPointer,
                TrackingDeleter<int>(&deleted));

            KITSUNE_UNUSED(pointer);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, CopyAssign)
    {
        int* rawPointer = Memory::New<int>(4);
        int* rawPointer2 = Memory::New<int>(5);

        std::vector<void*> deleted;

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
            SharedPtr<int> copy(rawPointer2, TrackingDeleter<int>(&deleted));

            copy = pointer;

            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 2);
        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer);
    }

    TEST(SharedPtrTest, TemplatedCopyAssign)
    {
        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        std::vector<void*> deleted;

        {
            SharedPtr<Derived> pointer(rawPointer, TrackingDeleter<Derived>(&deleted));
            SharedPtr<Base> copy(rawPointer2, TrackingDeleter<Derived>(&deleted));

            copy = pointer;

            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 2);
        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer);
    }

    TEST(SharedPtrTest, MoveAssign)
    {
        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>(5);
        int* rawPointer2 = Memory::New<int>(5);

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
            SharedPtr<int> moved(rawPointer2, TrackingDeleter<int>(&deleted));

            moved = std::move(pointer);

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 2);
        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer);
    }

    TEST(SharedPtrTest, TemplatedMoveAssign)
    {
        std::vector<void*> deleted;
        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        {
            SharedPtr<Derived> pointer(rawPointer, TrackingDeleter<Derived>(&deleted));
            SharedPtr<Base> moved(rawPointer2, TrackingDeleter<Base>(&deleted));

            moved = std::move(pointer);

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 2);
        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer);
    }

    TEST(SharedPtrTest, ScopedPtrAssign)
    {
        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Base>();

        std::vector<void*> deleted;

        {
            ScopedPtr<Derived, TrackingDeleter<Derived>> pointer(
                rawPointer,
                TrackingDeleter<Derived>(&deleted));

            SharedPtr<Base> moved(rawPointer2, TrackingDeleter<Base>(&deleted));
            moved = std::move(pointer);

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
        }

        EXPECT_EQ(deleted.size(), 2);
        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer);
    }

    TEST(SharedPtrTest, Reset)
    {
        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>();

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
            ASSERT_EQ(pointer.Get(), rawPointer);

            pointer.Reset();
            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, ResetPointer)
    {
        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
            ASSERT_EQ(pointer.Get(), rawPointer);

            pointer.Reset(rawPointer2);
            EXPECT_EQ(pointer.Get(), rawPointer2);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TEST(SharedPtrTest, ResetPointerDeleterAndAllocator)
    {
        std::vector<void*> deleted;
        std::vector<void*> deleted2;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        {
            SharedPtr<int> pointer(
                rawPointer,
                TrackingDeleter<int>(&deleted),
                TrackingAllocator());

            ASSERT_EQ(pointer.Get(), rawPointer);

            pointer.Reset(
                rawPointer2,
                TrackingDeleter<int>(&deleted2),
                TrackingAllocator());

            EXPECT_EQ(pointer.Get(), rawPointer2);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);

        EXPECT_EQ(deleted2.size(), 1);
        EXPECT_EQ(deleted2[0], rawPointer2);
    }

    TEST(SharedPtrTest, ResetPointerAndDeleter)
    {
        std::vector<void*> deleted;
        std::vector<void*> deleted2;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        {
            SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
            ASSERT_EQ(pointer.Get(), rawPointer);

            pointer.Reset(rawPointer2, TrackingDeleter<int>(&deleted2));
            EXPECT_EQ(pointer.Get(), rawPointer2);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);

        EXPECT_EQ(deleted2.size(), 1);
        EXPECT_EQ(deleted2[0], rawPointer2);
    }

    TEST(SharedPtrTest, Dereference)
    {
        int* rawPointer = Memory::New<int>();
        SharedPtr<int> pointer(rawPointer);

        EXPECT_EQ(&*pointer, rawPointer);
    }

    TEST(SharedPtrTest, Boolean)
    {
        int* rawPointer = Memory::New<int>();

        SharedPtr<int> pointer(rawPointer);
        SharedPtr<int> empty;

        EXPECT_TRUE((bool)pointer);
        EXPECT_FALSE((bool)empty);
    }

    /* SharedPtr<T>::Get() and SharedPtr<T>::GetCount() are assumed to work. */

    TEST(SharedPtrTest, Swap)
    {
        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        SharedPtr<int> pointer(rawPointer);
        SharedPtr<int> pointer2(rawPointer2);

        Usize count = pointer.GetCount();
        Usize count2 = pointer2.GetCount();

        pointer.Swap(pointer2);

        EXPECT_EQ(pointer.Get(), rawPointer2);
        EXPECT_EQ(pointer2.Get(), rawPointer);

        EXPECT_EQ(pointer.GetCount(), count2);
        EXPECT_EQ(pointer2.GetCount(), count);
    }

    TEST(SharedPtrTest, Comparisons)
    {
        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        SharedPtr<int> pointer(rawPointer);
        SharedPtr<int> pointer2(rawPointer2);

        EXPECT_EQ(pointer == pointer2, false);
        EXPECT_EQ(pointer == pointer, true);
        EXPECT_EQ(pointer2 == pointer2, true);

        /* operator!= is auto-generated by the compiler if operator== is defined. */

        EXPECT_EQ(pointer > pointer, pointer.Get() > pointer.Get());
        EXPECT_EQ(pointer < pointer, pointer.Get() < pointer.Get());
        EXPECT_EQ(pointer >= pointer, pointer.Get() >= pointer.Get());
        EXPECT_EQ(pointer <= pointer, pointer.Get() <= pointer.Get());

        EXPECT_EQ(pointer > pointer2, pointer.Get() > pointer2.Get());
        EXPECT_EQ(pointer < pointer2, pointer.Get() < pointer2.Get());
        EXPECT_EQ(pointer >= pointer2, pointer.Get() >= pointer2.Get());
        EXPECT_EQ(pointer <= pointer2, pointer.Get() <= pointer2.Get());
    }

    TEST(SharedPtrTest, NullptrComparisons)
    {
        int* null = nullptr;
        int* rawPointer = Memory::New<int>();

        SharedPtr<int> pointer(rawPointer);

        /* operator!= is auto-generated by the compiler if operator== is defined. */

        EXPECT_EQ(pointer == nullptr, pointer.Get() == null);
        EXPECT_EQ(pointer < nullptr, pointer.Get() < null);
        EXPECT_EQ(pointer > nullptr, pointer.Get() > null);
        EXPECT_EQ(pointer <= nullptr, pointer.Get() <= null);
        EXPECT_EQ(pointer >= nullptr, pointer.Get() >= null);

        EXPECT_EQ(nullptr == pointer, null == pointer.Get());
        EXPECT_EQ(nullptr < pointer, null < pointer.Get());
        EXPECT_EQ(nullptr > pointer, null > pointer.Get());
        EXPECT_EQ(nullptr <= pointer, null <= pointer.Get());
        EXPECT_EQ(nullptr >= pointer, null >= pointer.Get());
    }

    TEST(WeakPtrTest, DefaultConstructor)
    {
        WeakPtr<int> pointer;
        EXPECT_EQ(pointer.GetCount(), 0);
        EXPECT_EQ(pointer.Lock(), SharedPtr<int>());
    }

    TEST(WeakPtrTest, SharedPtrConstructor)
    {
        int* rawPointer = Memory::New<int>();

        SharedPtr<int> null;
        SharedPtr<int> pointer(rawPointer);

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        SharedPtr<int> copy = pointer;
        KITSUNE_UNUSED(copy);

        WeakPtr<int> nullWeakPointer = null;
        EXPECT_EQ(nullWeakPointer.GetCount(), null.GetCount());
        EXPECT_EQ(nullWeakPointer.Lock(), null);

        WeakPtr<int> weakPointer = pointer;
        EXPECT_EQ(weakPointer.GetCount(), pointer.GetCount());
        EXPECT_EQ(weakPointer.Lock(), pointer);
    }

    TEST(WeakPtrTest, CopyConstructor)
    {
        int* rawPointer = Memory::New<int>(5);
        SharedPtr<int> pointer(rawPointer);

        WeakPtr<int> weakPointer = pointer;
        ASSERT_EQ(weakPointer.GetCount(), 1);
        ASSERT_EQ(weakPointer.Lock(), pointer);

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        WeakPtr<int> weakCopy = weakPointer;
        EXPECT_EQ(weakCopy.GetCount(), 1);
        EXPECT_EQ(weakCopy.Lock(), pointer);
    }

    TEST(WeakPtrTest, TemplatedCopyConstructor)
    {
        auto* rawPointer = Memory::New<Derived>();
        SharedPtr<Derived> pointer(rawPointer);

        WeakPtr<Derived> weakPointer = pointer;
        ASSERT_EQ(weakPointer.GetCount(), 1);
        ASSERT_EQ(weakPointer.Lock(), pointer);

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        WeakPtr<Base> weakCopy = weakPointer;
        EXPECT_EQ(weakCopy.GetCount(), 1);
        EXPECT_EQ(weakCopy.Lock(), pointer);
    }

    TEST(WeakPtrTest, MoveConstructor)
    {
        int* rawPointer = Memory::New<int>();
        SharedPtr<int> pointer(rawPointer);

        WeakPtr<int> weak = pointer;
        ASSERT_EQ(weak.GetCount(), 1);
        ASSERT_EQ(weak.Lock(), pointer);

        WeakPtr<int> move = std::move(weak);
        EXPECT_EQ(move.GetCount(), 1);
        EXPECT_EQ(weak.GetCount(), 0);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), SharedPtr<int>());
    }

    TEST(WeakPtrTest, TemplatedMoveConstructor)
    {
        auto* rawPointer = Memory::New<Derived>();
        SharedPtr<Derived> pointer(rawPointer);

        WeakPtr<Derived> weak = pointer;
        ASSERT_EQ(weak.GetCount(), 1);
        ASSERT_EQ(weak.Lock(), pointer);

        WeakPtr<Base> move = std::move(weak);
        EXPECT_EQ(move.GetCount(), 1);
        EXPECT_EQ(weak.GetCount(), 0);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), SharedPtr<Base>());
    }

    TEST(WeakPtrTest, Destructor)
    {
        // No way of testing the destructor.
        EXPECT_TRUE(true);
    }

    TEST(WeakPtrTest, SharedPtrAssign)
    {
        WeakPtr<Base> weak;

        {
            auto* rawPointer = Memory::New<Derived>();
            SharedPtr<Derived> pointer(rawPointer);

            weak = pointer;

            EXPECT_EQ(weak.GetCount(), 1);
            EXPECT_EQ(weak.Lock(), pointer);
        }

        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(weak.Lock(), nullptr);
    }

    TEST(WeakPtrTest, CopyAssign)
    {
        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        SharedPtr<int> pointer(rawPointer);
        SharedPtr<int> pointer2(rawPointer2);

        WeakPtr<int> weak = pointer;
        WeakPtr<int> copy = pointer2;

        copy = weak;

        EXPECT_EQ(weak.GetCount(), 1);
        EXPECT_EQ(copy.GetCount(), 1);

        EXPECT_EQ(copy.Lock(), pointer);
    }

    TEST(WeakPtrTest, TemplatedCopyAssign)
    {
        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        SharedPtr<Derived> pointer(rawPointer);
        SharedPtr<Base> pointer2(rawPointer2);

        WeakPtr<Derived> weak = pointer;
        WeakPtr<Base> copy = pointer2;

        copy = weak;

        EXPECT_EQ(weak.GetCount(), 1);
        EXPECT_EQ(copy.GetCount(), 1);

        EXPECT_EQ(copy.Lock(), pointer);
    }

    TEST(WeakPtrTest, MoveAssign)
    {
        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        SharedPtr<int> pointer(rawPointer);
        SharedPtr<int> pointer2(rawPointer2);

        WeakPtr<int> weak = pointer;
        WeakPtr<int> move = pointer2;

        move = std::move(weak);

        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(move.GetCount(), 1);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), nullptr);
    }

    TEST(WeakPtrTest, TemplatedMoveAssign)
    {
        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        SharedPtr<Derived> pointer(rawPointer);
        SharedPtr<Base> pointer2(rawPointer2);

        WeakPtr<Derived> weak = pointer;
        WeakPtr<Base> move = pointer2;

        move = std::move(weak);

        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(move.GetCount(), 1);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), nullptr);
    }

    TEST(WeakPtrTest, Reset)
    {
        SharedPtr<int> pointer = MakeShared<int>();
        WeakPtr<int> weak = pointer;

        weak.Reset();
        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(weak.Lock(), SharedPtr<int>());
    }

    /* WeakPtr<T>::GetCount() is assumed to work. */

    TEST(WeakPtrTest, IsExpired)
    {
        WeakPtr<Base> weakPointer;
        WeakPtr<Base> null;

        SharedPtr<Derived> pointer2(Memory::New<Derived>());
        WeakPtr<Base> weakPointer2 = pointer2;

        {
            auto* rawPointer = Memory::New<Derived>();
            SharedPtr<Derived> pointer(rawPointer);

            weakPointer = pointer;
        }

        EXPECT_TRUE(weakPointer.IsExpired());
        EXPECT_TRUE(null.IsExpired());

        EXPECT_FALSE(weakPointer2.IsExpired());
    }

    TEST(WeakPtrTest, Swap)
    {
        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        SharedPtr<int> pointer(rawPointer);
        SharedPtr<int> pointer2(rawPointer2);

        WeakPtr<int> weakPtr = pointer;
        WeakPtr<int> weakPointer2 = pointer2;

        weakPtr.Swap(weakPointer2);

        EXPECT_EQ(weakPtr.Lock().Get(), rawPointer2);
        EXPECT_EQ(weakPointer2.Lock().Get(), rawPointer);
    }

    TEST(SharedPtrTest, StaticPointerCast)
    {
        SharedPtr<void> voidPointer = SharedPtr<void>(Memory::New<int>(3));
        SharedPtr<int> intPointer = StaticPointerCast<int>(voidPointer);

        EXPECT_EQ((void*)intPointer.Get(), voidPointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(voidPointer.GetCount(), 2);
    }

    TEST(SharedPtrTest, MoveStaticPointerCast)
    {
        SharedPtr<void> voidPointer = SharedPtr<void>(Memory::New<int>(3));
        void* rawPointer = voidPointer.Get();

        SharedPtr<int> intPointer = StaticPointerCast<int>(Move(voidPointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(voidPointer.GetCount(), 0);
    }

    TEST(SharedPtrTest, DynamicPointerCast)
    {
        SharedPtr<Base> pointer = MakeShared<Derived>();
        SharedPtr<Derived> intPointer = DynamicPointerCast<Derived>(pointer);

        EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(pointer.GetCount(), 2);
    }

    TEST(SharedPtrTest, MoveDynamicPointerCast)
    {
        SharedPtr<Base> pointer = MakeShared<Derived>();
        Base* rawPointer = pointer.Get();

        SharedPtr<Derived> intPointer = DynamicPointerCast<Derived>(Move(pointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }

    TEST(SharedPtrTest, ConstPointerCast)
    {
        SharedPtr<const int> pointer = MakeShared<int>();
        SharedPtr<int> intPointer = ConstPointerCast<int>(pointer);

        EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(pointer.GetCount(), 2);
    }

    TEST(SharedPtrTest, MoveConstPointerCast)
    {
        SharedPtr<const int> pointer = MakeShared<int>();
        const int* rawPointer = pointer.Get();

        SharedPtr<int> intPointer = ConstPointerCast<int>(Move(pointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }

    TEST(SharedPtrTest, ReinterpretPointerCast)
    {
        SharedPtr<int> pointer = MakeShared<int>();
        SharedPtr<float> intPointer = ReinterpretPointerCast<float>(pointer);

        EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(pointer.GetCount(), 2);
    }

    TEST(SharedPtrTest, MoveReinterpretPointerCast)
    {
        SharedPtr<int> pointer = MakeShared<int>();
        int* rawPointer = pointer.Get();

        SharedPtr<float> intPointer = ReinterpretPointerCast<float>(Move(pointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }
}

