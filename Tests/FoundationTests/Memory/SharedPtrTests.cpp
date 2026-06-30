#include <gtest/gtest.h>
#include "TrackingAllocator.h"

#include "Foundation/Memory/SharedPtr.h"

namespace
{
    using Kitsune::ThreadSafety;
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

    template<typename T>
    class SharedPtrTest : public ::testing::Test
    {
    public:
        template<typename U>
        using Ptr = Kitsune::SharedPtr<U, T::value>;

        template<typename U>
        using Weak = Kitsune::WeakPtr<U, T::value>;
    };

    template<typename T>
    class WeakPtrTest : public ::testing::Test
    {
    public:
        template<typename U>
        using Ptr = Kitsune::SharedPtr<U, T::value>;

        template<typename U>
        using Weak = Kitsune::WeakPtr<U, T::value>;
    };

    using SharedPtrTestTypes = ::testing::Types<
        std::integral_constant<ThreadSafety, ThreadSafety::NotThreadSafe>,
        std::integral_constant<ThreadSafety, ThreadSafety::ThreadSafe>>;

    TYPED_TEST_SUITE(SharedPtrTest, SharedPtrTestTypes);
    TYPED_TEST_SUITE(WeakPtrTest, SharedPtrTestTypes);

    TYPED_TEST(SharedPtrTest, DefaultNullptrConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        Ptr pointer;
        Ptr null = nullptr;

        EXPECT_EQ(pointer.Get(), nullptr);
        EXPECT_EQ(null.Get(), nullptr);

        EXPECT_EQ(pointer.GetCount(), 0);
        EXPECT_EQ(null.GetCount(), 0);
    }

    TYPED_TEST(SharedPtrTest, PointerConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(234);
        Ptr pointer(rawPointer);

        EXPECT_EQ(pointer.Get(), rawPointer);
        EXPECT_EQ(pointer.GetCount(), 1);

        // rawPointer here should've been destroyed by GlobalAllocator.
    }

    TYPED_TEST(SharedPtrTest, PointerDeleterConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer1 = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);

            Ptr moved(rawPointer1, TrackingDeleter<int>(&deleted));
            Ptr copied(rawPointer2, deleter);

            EXPECT_EQ(moved.Get(), rawPointer1);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(copied.Get(), rawPointer2);
            EXPECT_EQ(copied.GetCount(), 1);
        }

        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer1);
    }

    TYPED_TEST(SharedPtrTest, NullptrDeleterConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;
        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);

            Ptr moved(nullptr, TrackingDeleter<int>(&deleted));
            Ptr copied(nullptr, deleter);

            EXPECT_EQ(moved.Get(), nullptr);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(copied.Get(), nullptr);
            EXPECT_EQ(copied.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TYPED_TEST(SharedPtrTest, CopyDeleterAndAllocatorConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);
            Ptr pointer(rawPointer, deleter, TrackingAllocator());

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, MoveDeleterAndAllocatorConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        std::vector<void*> deleted;

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted),
                        TrackingAllocator());

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, NullptrCopyDeleterAndAllocatorConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;
        std::vector<void*> deleted;

        {
            TrackingDeleter<int> deleter(&deleted);
            Ptr pointer(nullptr, deleter, TrackingAllocator());

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TYPED_TEST(SharedPtrTest, NullptrMoveDeleterAndAllocatorConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;
        std::vector<void*> deleted;

        {
            Ptr pointer(nullptr, TrackingDeleter<int>(&deleted),
                        TrackingAllocator());

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TYPED_TEST(SharedPtrTest, CopyConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(4);
        std::vector<void*> deleted;

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            Ptr copy = pointer;

            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, AliasingCopyConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(45);
        std::vector<void*> deleted;

        {
            Ptr ownedPointer(rawPointer, TrackingDeleter<int>(&deleted));

            int* storedPointer = reinterpret_cast<int*>(0xDEADC0DE);
            Ptr pointer(ownedPointer, storedPointer);

            EXPECT_EQ(pointer.Get(), storedPointer);
            EXPECT_EQ(pointer.GetCount(), 2);

            EXPECT_EQ(ownedPointer.Get(), rawPointer);
            EXPECT_EQ(ownedPointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, TemplatedCopyConstructor)
    {
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;
        using BasePtr = typename TestFixture::template Ptr<Base>;

        auto* rawPointer = Memory::New<Derived>();
        std::vector<void*> deleted;

        {
            DerivedPtr pointer(rawPointer, TrackingDeleter<Derived>(&deleted));

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            BasePtr copy = pointer;

            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, TemplatedCopyConstructorHandlesNullptr)
    {
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;
        using BasePtr = typename TestFixture::template Ptr<Base>;

        std::vector<void*> deleted;

        {
            DerivedPtr pointer(nullptr, TrackingDeleter<Derived>(&deleted));

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            BasePtr copy = pointer;

            EXPECT_EQ(copy.Get(), nullptr);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 0);
    }

    TYPED_TEST(SharedPtrTest, MoveConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>(5);

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
            Ptr moved = std::move(pointer);

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, TemplatedMoveConstructor)
    {
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;
        using BasePtr = typename TestFixture::template Ptr<Base>;

        std::vector<void*> deleted;
        auto* rawPointer = Memory::New<Derived>();

        {
            DerivedPtr pointer(rawPointer, TrackingDeleter<Derived>(&deleted));
            BasePtr moved(std::move(pointer));

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, AliasingMoveConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(45);
        std::vector<void*> deleted;

        {
            Ptr ownedPointer(rawPointer, TrackingDeleter<int>(&deleted));

            int* storedPointer = reinterpret_cast<int*>(0xDEADC0DE);
            Ptr pointer(std::move(ownedPointer), storedPointer);

            EXPECT_EQ(pointer.Get(), storedPointer);
            EXPECT_EQ(pointer.GetCount(), 1);

            EXPECT_EQ(ownedPointer.Get(), nullptr);
            EXPECT_EQ(ownedPointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, WeakPtrConstructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;
        using Weak = typename TestFixture::template Weak<int>;

        int* rawPointer = Memory::New<int>(45);
        std::vector<void*> deleted;

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
            Weak weakPointer(pointer);

            ASSERT_EQ(pointer.GetCount(), 1);
            ASSERT_EQ(pointer.Get(), rawPointer);

            Ptr copy(weakPointer);
            EXPECT_EQ(copy.Get(), rawPointer);
            EXPECT_EQ(copy.GetCount(), 2);

            EXPECT_EQ(pointer.Get(), rawPointer);
            EXPECT_EQ(pointer.GetCount(), 2);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);

        Weak weakNull;
        ASSERT_TRUE(weakNull.IsExpired());

        EXPECT_THROW(
            Ptr pointer(weakNull),
            BadWeakPtrException);
    }

    TYPED_TEST(SharedPtrTest, ScopedPtrConstructor)
    {
        using BasePtr = typename TestFixture::template Ptr<Base>;

        auto* rawPointer = Memory::New<Derived>();
        std::vector<void*> deleted;

        {
            ScopedPtr<Derived, TrackingDeleter<Derived>> pointer(
                rawPointer,
                TrackingDeleter<Derived>(&deleted));

            BasePtr moved(std::move(pointer));
            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, Destructor)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(5);
        std::vector<void*> deleted;

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
            KITSUNE_UNUSED(pointer);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, CopyAssign)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(4);
        int* rawPointer2 = Memory::New<int>(5);

        std::vector<void*> deleted;

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
            Ptr copy(rawPointer2, TrackingDeleter<int>(&deleted));

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

    TYPED_TEST(SharedPtrTest, TemplatedCopyAssign)
    {
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;
        using BasePtr = typename TestFixture::template Ptr<Base>;

        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        std::vector<void*> deleted;

        {
            DerivedPtr pointer(rawPointer, TrackingDeleter<Derived>(&deleted));
            BasePtr copy(rawPointer2, TrackingDeleter<Derived>(&deleted));

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

    TYPED_TEST(SharedPtrTest, MoveAssign)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>(5);
        int* rawPointer2 = Memory::New<int>(5);

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
            Ptr moved(rawPointer2, TrackingDeleter<int>(&deleted));

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

    TYPED_TEST(SharedPtrTest, TemplatedMoveAssign)
    {
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;
        using BasePtr = typename TestFixture::template Ptr<Base>;

        std::vector<void*> deleted;
        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        {
            DerivedPtr pointer(rawPointer, TrackingDeleter<Derived>(&deleted));
            BasePtr moved(rawPointer2, TrackingDeleter<Base>(&deleted));

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

    TYPED_TEST(SharedPtrTest, ScopedPtrAssign)
    {
        using BasePtr = typename TestFixture::template Ptr<Base>;

        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Base>();

        std::vector<void*> deleted;

        {
            ScopedPtr<Derived, TrackingDeleter<Derived>> pointer(
                rawPointer,
                TrackingDeleter<Derived>(&deleted));

            BasePtr moved(rawPointer2, TrackingDeleter<Base>(&deleted));
            moved = std::move(pointer);

            EXPECT_EQ(moved.Get(), rawPointer);
            EXPECT_EQ(moved.GetCount(), 1);

            EXPECT_EQ(pointer.Get(), nullptr);
        }

        EXPECT_EQ(deleted.size(), 2);
        EXPECT_EQ(deleted[0], rawPointer2);
        EXPECT_EQ(deleted[1], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, Reset)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>();

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
            ASSERT_EQ(pointer.Get(), rawPointer);

            pointer.Reset();
            EXPECT_EQ(pointer.Get(), nullptr);
            EXPECT_EQ(pointer.GetCount(), 0);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, ResetPointer)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        std::vector<void*> deleted;
        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
            ASSERT_EQ(pointer.Get(), rawPointer);

            pointer.Reset(rawPointer2);
            EXPECT_EQ(pointer.Get(), rawPointer2);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);
    }

    TYPED_TEST(SharedPtrTest, ResetPointerDeleterAndAllocator)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        std::vector<void*> deleted;
        std::vector<void*> deleted2;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted),
                        TrackingAllocator());

            ASSERT_EQ(pointer.Get(), rawPointer);

            pointer.Reset(rawPointer2, TrackingDeleter<int>(&deleted2),
                          TrackingAllocator());

            EXPECT_EQ(pointer.Get(), rawPointer2);
            EXPECT_EQ(pointer.GetCount(), 1);
        }

        EXPECT_EQ(deleted.size(), 1);
        EXPECT_EQ(deleted[0], rawPointer);

        EXPECT_EQ(deleted2.size(), 1);
        EXPECT_EQ(deleted2[0], rawPointer2);
    }

    TYPED_TEST(SharedPtrTest, ResetPointerAndDeleter)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        std::vector<void*> deleted;
        std::vector<void*> deleted2;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        {
            Ptr pointer(rawPointer, TrackingDeleter<int>(&deleted));
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

    TYPED_TEST(SharedPtrTest, Dereference)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        Ptr pointer(rawPointer);

        EXPECT_EQ(&*pointer, rawPointer);
    }

    TYPED_TEST(SharedPtrTest, Boolean)
    {
        using Ptr = typename TestFixture::template Ptr<int>;
        int* rawPointer = Memory::New<int>();

        Ptr pointer(rawPointer);
        Ptr empty;

        EXPECT_TRUE((bool)pointer);
        EXPECT_FALSE((bool)empty);
    }

    /* Ptr<T>::Get() and Ptr<T>::GetCount() are assumed to work. */

    TYPED_TEST(SharedPtrTest, Swap)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        Ptr pointer(rawPointer);
        Ptr pointer2(rawPointer2);

        Usize count = pointer.GetCount();
        Usize count2 = pointer2.GetCount();

        pointer.Swap(pointer2);

        EXPECT_EQ(pointer.Get(), rawPointer2);
        EXPECT_EQ(pointer2.Get(), rawPointer);

        EXPECT_EQ(pointer.GetCount(), count2);
        EXPECT_EQ(pointer2.GetCount(), count);
    }

    TYPED_TEST(SharedPtrTest, Comparisons)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        Ptr pointer(rawPointer);
        Ptr pointer2(rawPointer2);

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

    TYPED_TEST(SharedPtrTest, NullptrComparisons)
    {
        using Ptr = typename TestFixture::template Ptr<int>;

        int* null = nullptr;
        int* rawPointer = Memory::New<int>();

        Ptr pointer(rawPointer);

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

    TYPED_TEST(WeakPtrTest, DefaultConstructor)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        Weak pointer;
        EXPECT_EQ(pointer.GetCount(), 0);
        EXPECT_EQ(pointer.Lock(), Ptr{});
    }

    TYPED_TEST(WeakPtrTest, SharedPtrConstructor)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();

        Ptr null;
        Ptr pointer(rawPointer);

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        Ptr copy = pointer;
        KITSUNE_UNUSED(copy);

        Weak nullWeakPointer = null;
        EXPECT_EQ(nullWeakPointer.GetCount(), null.GetCount());
        EXPECT_EQ(nullWeakPointer.Lock(), null);

        Weak weakPointer = pointer;
        EXPECT_EQ(weakPointer.GetCount(), pointer.GetCount());
        EXPECT_EQ(weakPointer.Lock(), pointer);
    }

    TYPED_TEST(WeakPtrTest, CopyConstructor)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(5);
        Ptr pointer(rawPointer);

        Weak weakPointer = pointer;
        ASSERT_EQ(weakPointer.GetCount(), 1);
        ASSERT_EQ(weakPointer.Lock(), pointer);

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        Weak weakCopy = weakPointer;
        EXPECT_EQ(weakCopy.GetCount(), 1);
        EXPECT_EQ(weakCopy.Lock(), pointer);
    }

    TYPED_TEST(WeakPtrTest, TemplatedCopyConstructor)
    {
        using BaseWeak = typename TestFixture::template Weak<Base>;
        using DerivedWeak = typename TestFixture::template Weak<Derived>;

        using Ptr = typename TestFixture::template Ptr<Derived>;

        auto* rawPointer = Memory::New<Derived>();
        Ptr pointer(rawPointer);

        DerivedWeak weakPointer = pointer;
        ASSERT_EQ(weakPointer.GetCount(), 1);
        ASSERT_EQ(weakPointer.Lock(), pointer);

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        BaseWeak weakCopy = weakPointer;
        EXPECT_EQ(weakCopy.GetCount(), 1);
        EXPECT_EQ(weakCopy.Lock(), pointer);
    }

    TYPED_TEST(WeakPtrTest, MoveConstructor)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        Ptr pointer(rawPointer);

        Weak weak = pointer;
        ASSERT_EQ(weak.GetCount(), 1);
        ASSERT_EQ(weak.Lock(), pointer);

        Weak move = std::move(weak);
        EXPECT_EQ(move.GetCount(), 1);
        EXPECT_EQ(weak.GetCount(), 0);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), Ptr{});
    }

    TYPED_TEST(WeakPtrTest, TemplatedMoveConstructor)
    {
        using BaseWeak = typename TestFixture::template Weak<Base>;
        using DerivedWeak = typename TestFixture::template Weak<Derived>;

        using BasePtr = typename TestFixture::template Ptr<Base>;
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;

        auto* rawPointer = Memory::New<Derived>();
        DerivedPtr pointer(rawPointer);

        DerivedWeak weak = pointer;
        ASSERT_EQ(weak.GetCount(), 1);
        ASSERT_EQ(weak.Lock(), pointer);

        BaseWeak move = std::move(weak);
        EXPECT_EQ(move.GetCount(), 1);
        EXPECT_EQ(weak.GetCount(), 0);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), BasePtr{});
    }

    TYPED_TEST(WeakPtrTest, Destructor)
    {
        // No way of testing the destructor.
        EXPECT_TRUE(true);
    }

    TYPED_TEST(WeakPtrTest, SharedPtrAssign)
    {
        using Weak = typename TestFixture::template Weak<Base>;
        using Ptr = typename TestFixture::template Ptr<Derived>;

        Weak weak;

        {
            auto* rawPointer = Memory::New<Derived>();
            Ptr pointer(rawPointer);

            weak = pointer;

            EXPECT_EQ(weak.GetCount(), 1);
            EXPECT_EQ(weak.Lock(), pointer);
        }

        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(weak.Lock(), nullptr);
    }

    TYPED_TEST(WeakPtrTest, CopyAssign)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        Ptr pointer(rawPointer);
        Ptr pointer2(rawPointer2);

        Weak weak = pointer;
        Weak copy = pointer2;

        copy = weak;

        EXPECT_EQ(weak.GetCount(), 1);
        EXPECT_EQ(copy.GetCount(), 1);

        EXPECT_EQ(copy.Lock(), pointer);
    }

    TYPED_TEST(WeakPtrTest, TemplatedCopyAssign)
    {
        using BaseWeak = typename TestFixture::template Weak<Base>;
        using DerivedWeak = typename TestFixture::template Weak<Derived>;

        using BasePtr = typename TestFixture::template Ptr<Base>;
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;

        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        DerivedPtr pointer(rawPointer);
        BasePtr pointer2(rawPointer2);

        DerivedWeak weak = pointer;
        BaseWeak copy = pointer2;

        copy = weak;

        EXPECT_EQ(weak.GetCount(), 1);
        EXPECT_EQ(copy.GetCount(), 1);

        EXPECT_EQ(copy.Lock(), pointer);
    }

    TYPED_TEST(WeakPtrTest, MoveAssign)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        Ptr pointer(rawPointer);
        Ptr pointer2(rawPointer2);

        Weak weak = pointer;
        Weak move = pointer2;

        move = std::move(weak);

        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(move.GetCount(), 1);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), nullptr);
    }

    TYPED_TEST(WeakPtrTest, TemplatedMoveAssign)
    {
        using BaseWeak = typename TestFixture::template Weak<Base>;
        using DerivedWeak = typename TestFixture::template Weak<Derived>;

        using BasePtr = typename TestFixture::template Ptr<Base>;
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;

        auto* rawPointer = Memory::New<Derived>();
        auto* rawPointer2 = Memory::New<Derived>();

        DerivedPtr pointer(rawPointer);
        BasePtr pointer2(rawPointer2);

        DerivedWeak weak = pointer;
        BaseWeak move = pointer2;

        move = std::move(weak);

        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(move.GetCount(), 1);

        EXPECT_EQ(move.Lock(), pointer);
        EXPECT_EQ(weak.Lock(), nullptr);
    }

    TYPED_TEST(WeakPtrTest, Reset)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>(4);

        Ptr pointer(rawPointer);
        Weak weak = pointer;

        weak.Reset();
        EXPECT_EQ(weak.GetCount(), 0);
        EXPECT_EQ(weak.Lock(), Ptr{});
    }

    /* WeakPtr<T>::GetCount() is assumed to work. */

    TYPED_TEST(WeakPtrTest, IsExpired)
    {
        using BaseWeak = typename TestFixture::template Weak<Base>;
        using DerivedWeak = typename TestFixture::template Weak<Derived>;

        using Ptr = typename TestFixture::template Ptr<Derived>;

        BaseWeak weakPointer;
        BaseWeak null;

        Ptr pointer2(Memory::New<Derived>());
        BaseWeak weakPointer2 = pointer2;

        {
            auto* rawPointer = Memory::New<Derived>();
            Ptr pointer(rawPointer);

            weakPointer = pointer;
        }

        EXPECT_TRUE(weakPointer.IsExpired());
        EXPECT_TRUE(null.IsExpired());

        EXPECT_FALSE(weakPointer2.IsExpired());
    }

    TYPED_TEST(WeakPtrTest, Swap)
    {
        using Weak = typename TestFixture::template Weak<int>;
        using Ptr = typename TestFixture::template Ptr<int>;

        int* rawPointer = Memory::New<int>();
        int* rawPointer2 = Memory::New<int>();

        Ptr pointer(rawPointer);
        Ptr pointer2(rawPointer2);

        Weak weakPtr = pointer;
        Weak weakPointer2 = pointer2;

        weakPtr.Swap(weakPointer2);

        EXPECT_EQ(weakPtr.Lock().Get(), rawPointer2);
        EXPECT_EQ(weakPointer2.Lock().Get(), rawPointer);
    }

    TYPED_TEST(SharedPtrTest, StaticPointerCast)
    {
        using VoidPtr = typename TestFixture::template Ptr<void>;
        using IntPtr = typename TestFixture::template Ptr<int>;

        VoidPtr voidPointer(Memory::New<int>(3));
        IntPtr intPointer = StaticPointerCast<int>(voidPointer);

        EXPECT_EQ((void*)intPointer.Get(), voidPointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(voidPointer.GetCount(), 2);
    }

    TYPED_TEST(SharedPtrTest, MoveStaticPointerCast)
    {
        using VoidPtr = typename TestFixture::template Ptr<void>;
        using IntPtr = typename TestFixture::template Ptr<int>;

        VoidPtr voidPointer(Memory::New<int>(3));
        void* rawPointer = voidPointer.Get();

        IntPtr intPointer = StaticPointerCast<int>(Move(voidPointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(voidPointer.GetCount(), 0);
    }

    TYPED_TEST(SharedPtrTest, DynamicPointerCast)
    {
        using BasePtr = typename TestFixture::template Ptr<Base>;
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;

        BasePtr pointer(Memory::New<Derived>());
        DerivedPtr intPointer = DynamicPointerCast<Derived>(pointer);

        EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(pointer.GetCount(), 2);
    }

    TYPED_TEST(SharedPtrTest, MoveDynamicPointerCast)
    {
        using BasePtr = typename TestFixture::template Ptr<Base>;
        using DerivedPtr = typename TestFixture::template Ptr<Derived>;

        BasePtr pointer(Memory::New<Derived>());
        Base* rawPointer = pointer.Get();

        DerivedPtr intPointer = DynamicPointerCast<Derived>(Move(pointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }

    TYPED_TEST(SharedPtrTest, ConstPointerCast)
    {
        using Ptr = typename TestFixture::template Ptr<int>;
        using ConstPtr = typename TestFixture::template Ptr<const int>;

        ConstPtr pointer(Memory::New<int>());
        Ptr intPointer = ConstPointerCast<int>(pointer);

        EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(pointer.GetCount(), 2);
    }

    TYPED_TEST(SharedPtrTest, MoveConstPointerCast)
    {
        using Ptr = typename TestFixture::template Ptr<int>;
        using ConstPtr = typename TestFixture::template Ptr<const int>;

        ConstPtr pointer(Memory::New<int>());
        const int* rawPointer = pointer.Get();

        Ptr intPointer = ConstPointerCast<int>(Move(pointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }

    TYPED_TEST(SharedPtrTest, ReinterpretPointerCast)
    {
        using IntPtr = typename TestFixture::template Ptr<int>;
        using FloatPtr = typename TestFixture::template Ptr<float>;

        IntPtr pointer(Memory::New<int>());
        FloatPtr intPointer = ReinterpretPointerCast<float>(pointer);

        EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

        EXPECT_EQ(intPointer.GetCount(), 2);
        EXPECT_EQ(pointer.GetCount(), 2);
    }

    TYPED_TEST(SharedPtrTest, MoveReinterpretPointerCast)
    {
        using IntPtr = typename TestFixture::template Ptr<int>;
        using FloatPtr = typename TestFixture::template Ptr<float>;

        IntPtr pointer(Memory::New<int>());
        int* rawPointer = pointer.Get();

        FloatPtr intPointer = ReinterpretPointerCast<float>(Move(pointer));
        EXPECT_EQ((void*)intPointer.Get(), rawPointer);

        EXPECT_EQ(intPointer.GetCount(), 1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }
}

