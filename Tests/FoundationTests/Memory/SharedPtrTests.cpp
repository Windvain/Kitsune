#include "Foundation/Memory/SharedPtr.h"

#include <gtest/gtest.h>
#include <algorithm>

using namespace Kitsune;

namespace
{
    template<typename T>
    class MyDeleter
    {
    public:
        using ValueType = T;

        MyDeleter() = default;
        MyDeleter(T** deletedPtr, int id = 0)
            : Id(id), m_DeletedPtr(deletedPtr)
        {
        }

        MyDeleter(const MyDeleter& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;
        }

        MyDeleter(MyDeleter&& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            deleter.Moved = true;

            deleter.m_DeletedPtr = nullptr;
            deleter.Id = 0;
        }
        ~MyDeleter() = default;

    public:
        MyDeleter& operator=(const MyDeleter& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            return *this;
        }

        template<typename U>
        MyDeleter& operator=(const MyDeleter<U>& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            return *this;
        }

        MyDeleter& operator=(MyDeleter&& deleter)
        {
            m_DeletedPtr = deleter.m_DeletedPtr;
            Id = deleter.Id;

            deleter.m_DeletedPtr = nullptr;
            deleter.Id = 0;

            return *this;
        }

        template<typename U>
        MyDeleter& operator=(MyDeleter<U>&& deleter)
        {
            m_DeletedPtr = (T**)deleter.m_DeletedPtr;
            Id = deleter.Id;

            deleter.m_DeletedPtr = nullptr;
            deleter.Id = 0;

            return *this;
        }

        void operator()(T* pointer)
        {
            if (m_DeletedPtr)
                *m_DeletedPtr = pointer;

            Kitsune::Memory::Delete(pointer);
        }

    public:
        int Id = 0;
        bool Moved = false;

    private:
        template<typename U>
        friend class MyDeleter;

        T** m_DeletedPtr;
    };

    class NotingAllocator
    {
    public:
        NotingAllocator() = default;
        NotingAllocator(void** allocator, void** freed)
            : m_Allocated(allocator), m_Freed(freed)
        {
        }

        NotingAllocator(const NotingAllocator&) = default;
        NotingAllocator& operator=(const NotingAllocator&) = default;

        NotingAllocator(NotingAllocator&& allocator)
            : m_Allocated(std::exchange(allocator.m_Allocated, nullptr)),
              m_Freed(std::exchange(allocator.m_Freed, nullptr))
        {
            allocator.Moved = true;
        }

        NotingAllocator& operator=(NotingAllocator&& allocator)
        {
            m_Allocated = std::exchange(allocator.m_Allocated, nullptr);
            m_Freed = std::exchange(allocator.m_Freed, nullptr);

            allocator.Moved = true;

            return *this;
        }

        ~NotingAllocator() = default;

    public:
        void* Allocate(Kitsune::Usize size,
                       Kitsune::Usize align = __STDCPP_DEFAULT_NEW_ALIGNMENT__)
        {
            void* pointer = Kitsune::Memory::Allocate(size, align);
            if (m_Allocated)
                *m_Allocated = pointer;

            return pointer;
        }

        void Free(void* pointer, Usize size)
        {
            if (m_Freed)
                *m_Freed = pointer;

            Memory::Free(pointer, size);
        }

    public:
        bool Moved = false;

    private:
        void** m_Allocated = nullptr;
        void** m_Freed = nullptr;
    };

    [[maybe_unused]]
    bool operator==(const NotingAllocator&, const NotingAllocator&)
    {
        return true;
    }

    class Base
    {
    public:
        virtual ~Base()
        {
        }
    };

    class Derived : public Base
    {
    };
}

TEST(SharedPtrTests, DefaultNullptrConstructor)
{
    SharedPtr<int> pointer;
    SharedPtr<int> null = nullptr;

    EXPECT_EQ(pointer.Get(), nullptr);
    EXPECT_EQ(null.Get(), nullptr);
}

TEST(SharedPtrTests, PointerConstructor)
{
    int* rawPointer = Memory::New<int>(234);
    auto pointer = SharedPtr<int>(rawPointer);

    EXPECT_EQ(pointer.Get(), rawPointer);
}

TEST(SharedPtrTests, DeleterConstructor)
{
    int* rawPointer1 = Memory::New<int>();
    int* rawPointer2 = Memory::New<int>();

    int* deleted1 = nullptr;
    int* deleted2 = nullptr;

    {
        MyDeleter<int> movedDeleter = MyDeleter<int>(&deleted1);
        MyDeleter<int> copiedDeleter = MyDeleter<int>(&deleted2);

        auto movedPointer = SharedPtr<int>(rawPointer1, std::move(movedDeleter));
        auto copiedPointer = SharedPtr<int>(rawPointer2, copiedDeleter);

        KITSUNE_UNUSED(movedPointer);
        KITSUNE_UNUSED(copiedPointer);
    }

    EXPECT_EQ(rawPointer1, deleted1);
    EXPECT_EQ(rawPointer2, deleted2);
}

TEST(SharedPtrTests, NullptrDeleterConstructor)
{
    int* deleted1 = nullptr;
    int* deleted2 = nullptr;

    {
        MyDeleter<int> movedDeleter = MyDeleter<int>(&deleted1);
        MyDeleter<int> copiedDeleter = MyDeleter<int>(&deleted2);

        auto movedPointer = SharedPtr<int>(nullptr, std::move(movedDeleter));
        auto copiedPointer = SharedPtr<int>(nullptr, copiedDeleter);

        KITSUNE_UNUSED(movedPointer);
        KITSUNE_UNUSED(copiedPointer);
    }

    EXPECT_EQ(deleted1, nullptr);
    EXPECT_EQ(deleted2, nullptr);
}

TEST(SharedPtrTests, CopyDeleterAndAllocatorConstructor)
{
    int* rawPointer = Memory::New<int>();
    int* deletedPointer = nullptr;

    void* allocatedPointer = nullptr;
    void* freedPointer = nullptr;

    {
        MyDeleter<int> deleter(&deletedPointer);
        NotingAllocator allocator(&allocatedPointer, &freedPointer);

        auto pointer = SharedPtr<int>(rawPointer, deleter, allocator);
        KITSUNE_UNUSED(pointer);
    }

    EXPECT_EQ(allocatedPointer, allocatedPointer);
    EXPECT_EQ(rawPointer, deletedPointer);
}

TEST(SharedPtrTests, MoveDeleterAndAllocatorConstructor)
{
    int* rawPointer = Memory::New<int>();
    int* deletedPointer = nullptr;

    void* allocatedPointer = nullptr;
    void* freedPointer = nullptr;

    {
        MyDeleter<int> deleter(&deletedPointer);
        NotingAllocator allocator(&allocatedPointer, &freedPointer);

        auto pointer = SharedPtr<int>(rawPointer, Move(deleter), allocator);
        KITSUNE_UNUSED(pointer);
    }

    EXPECT_EQ(allocatedPointer, allocatedPointer);
    EXPECT_EQ(rawPointer, deletedPointer);
}

TEST(SharedPtrTests, NullptrCopyDeleterAndAllocatorConstructor)
{
    int* deletedPointer = (int*)0xDEAD;

    void* allocatedPointer = nullptr;
    void* freedPointer = nullptr;

    {
        MyDeleter<int> deleter(&deletedPointer);
        NotingAllocator allocator(&allocatedPointer, &freedPointer);

        auto pointer = SharedPtr<int>(nullptr, deleter, allocator);
        KITSUNE_UNUSED(pointer);
    }

    EXPECT_EQ(allocatedPointer, allocatedPointer);
    EXPECT_EQ(deletedPointer, (int*)0xDEAD);
}

TEST(SharedPtrTests, NullptrMoveDeleterAndAllocatorConstructor)
{
    int* deletedPointer = (int*)0xDEAD;

    void* allocatedPointer = nullptr;
    void* freedPointer = nullptr;

    {
        MyDeleter<int> deleter(&deletedPointer);
        NotingAllocator allocator(&allocatedPointer, &freedPointer);

        auto pointer = SharedPtr<int>(nullptr, Move(deleter), allocator);
        KITSUNE_UNUSED(pointer);
    }

    EXPECT_EQ(allocatedPointer, allocatedPointer);
    EXPECT_EQ(deletedPointer, (int*)0xDEAD);
}

TEST(SharedPtrTests, CopyConstructor)
{
    auto null = SharedPtr<int>();
    auto pointer = MakeShared<int>(5);

    auto copiedNull = null;
    auto copiedPointer = pointer;

    EXPECT_EQ(copiedNull.Get(), nullptr);
    EXPECT_EQ(copiedPointer.Get(), pointer.Get());

    EXPECT_EQ(copiedNull.GetCount(), 0);
    EXPECT_EQ(copiedPointer.GetCount(), 2);
}

TEST(SharedPtrTests, AssignCopyConstructor)
{
    int* deletedPtr;
    int* expectedPtr = Memory::New<int>(45);

    {
        auto pointer = SharedPtr<int>(expectedPtr, MyDeleter<int>(&deletedPtr));

        auto rawPointer = (int*)0xDEAD;
        auto assigned = SharedPtr<int>(pointer, rawPointer);

        EXPECT_EQ(assigned.Get(), rawPointer);
        EXPECT_EQ(pointer.GetCount(), 2);
        EXPECT_EQ(assigned.GetCount(), 2);
    }

    EXPECT_EQ(deletedPtr, expectedPtr);      // Deleter gets called on 'firstPtr', not 0xDEAD.
}

TEST(SharedPtrTests, TemplatedCopyConstructor)
{
    auto null = SharedPtr<Derived>();
    auto pointer = MakeShared<Derived>();

    SharedPtr<Base> copiedNull = null;
    SharedPtr<Base> copiedPointer = pointer;

    EXPECT_EQ(copiedNull.Get(), nullptr);
    EXPECT_EQ(copiedPointer.Get(), pointer.Get());

    EXPECT_EQ(copiedNull.GetCount(), 0);
    EXPECT_EQ(copiedPointer.GetCount(), 2);
}

TEST(SharedPtrTests, MoveConstructor)
{
    int* rawPointer = Memory::New<int>(5);

    auto null = SharedPtr<int>();
    auto pointer = SharedPtr<int>(rawPointer);

    auto movedNull = std::move(null);
    auto movedPointer = std::move(pointer);

    EXPECT_EQ(movedNull.Get(), nullptr);
    EXPECT_EQ(movedPointer.Get(), rawPointer);

    EXPECT_EQ(movedNull.GetCount(), 0);
    EXPECT_EQ(movedPointer.GetCount(), 1);
}

TEST(SharedPtrTests, TemplatedMoveConstructor)
{
    Derived* rawPointer = Memory::New<Derived>();

    auto null = SharedPtr<Derived>();
    auto pointer = SharedPtr<Derived>(rawPointer);

    SharedPtr<Base> movedNull = std::move(null);
    SharedPtr<Base> movedPointer = std::move(pointer);

    EXPECT_EQ(movedNull.Get(), nullptr);
    EXPECT_EQ(null.GetCount(), 0);
    EXPECT_EQ(movedPointer.Get(), rawPointer);

    EXPECT_EQ(movedNull.GetCount(), 0);
    EXPECT_EQ(pointer.GetCount(), 0);
    EXPECT_EQ(movedPointer.GetCount(), 1);
}

TEST(SharedPtrTests, AssignMoveConstructor)
{
    int* deletedPtr;
    int* expectedPtr = Memory::New<int>(45);

    {
        auto pointer = SharedPtr<int>(expectedPtr, MyDeleter<int>(&deletedPtr));

        auto rawPointer = (int*)0xDEAD;
        auto assigned = SharedPtr<int>(Move(pointer), rawPointer);

        EXPECT_EQ(assigned.Get(), rawPointer);
        EXPECT_EQ(assigned.GetCount(), 1);

        EXPECT_EQ(pointer.GetCount(), 0);
    }

    EXPECT_EQ(deletedPtr, expectedPtr);      // Deleter gets called on 'firstPtr', not 0xDEAD.
}

TEST(SharedPtrTests, WeakPtrConstructor)
{

    SharedPtr<int> pointer = MakeShared<int>(1);
    SharedPtr<int> copy;

    {
        auto weak = WeakPtr<int>(pointer);
        EXPECT_EQ(pointer.GetCount(), 1);

        copy = SharedPtr<int>(weak);
        EXPECT_EQ(pointer.GetCount(), 2);
        EXPECT_EQ(copy.Get(), pointer.Get());
    }

    EXPECT_EQ(pointer.GetCount(), 2);
}

TEST(SharedPtrTests, ScopedPtrConstructor)
{
    Derived* rawPointer = Memory::New<Derived>();
    Derived* deleted = nullptr;

    ScopedPtr<Derived, MyDeleter<Derived>> pointer(
        rawPointer, MyDeleter<Derived>(&deleted));

    SharedPtr<Base> movedPointer = std::move(pointer);

    EXPECT_EQ(movedPointer.Get(), rawPointer);
    EXPECT_EQ(movedPointer.GetCount(), 1);
    EXPECT_EQ(pointer.Get(), nullptr);
}

TEST(SharedPtrTests, Destructor)
{
    int* rawPointer = Memory::New<int>(5);
    int* deleted = nullptr;

    {
        MyDeleter<int> deleter = MyDeleter<int>(&deleted);
        SharedPtr<int> pointer = SharedPtr<int>(rawPointer, std::move(deleter));

        KITSUNE_UNUSED(pointer);
    }

    EXPECT_EQ(rawPointer, deleted);
}

TEST(SharedPtrTests, CopyAssign)
{
    int* rawPointer1 = Memory::New<int>(5);
    int* rawPointer2 = Memory::New<int>(10);

    int* deleted = nullptr;
    int* deleted2 = nullptr;

    {
        auto pointer = SharedPtr<int>(rawPointer1, MyDeleter<int>(&deleted2));
        auto copy = SharedPtr<int>(rawPointer2, MyDeleter<int>(&deleted));

        copy = pointer;

        EXPECT_EQ(deleted, rawPointer2);
        EXPECT_EQ(copy.Get(), pointer.Get());
    }

    EXPECT_EQ(deleted2, rawPointer1);
}

TEST(SharedPtrTests, TemplateCopyAssign)
{
    Derived* rawPointer1 = Memory::New<Derived>();
    Base* rawPointer2 = Memory::New<Base>();

    Base* deleted = nullptr;
    Derived* deleted2 = nullptr;

    {
        auto pointer = SharedPtr<Derived>(rawPointer1, MyDeleter<Derived>(&deleted2));
        auto copy = SharedPtr<Base>(rawPointer2, MyDeleter<Base>(&deleted));

        copy = pointer;

        EXPECT_EQ(deleted, rawPointer2);
        EXPECT_EQ(copy.Get(), pointer.Get());
    }

    EXPECT_EQ(deleted2, rawPointer1);
}

TEST(SharedPtrTests, MoveAssign)
{
    int* rawPointer1 = Memory::New<int>(5);
    int* rawPointer2 = Memory::New<int>(10);

    int* deleted = nullptr;
    int* deleted2 = nullptr;

    {
        auto pointer = SharedPtr<int>(rawPointer1, MyDeleter<int>(&deleted2));
        auto move = SharedPtr<int>(rawPointer2, MyDeleter<int>(&deleted));

        move = std::move(pointer);

        EXPECT_EQ(deleted, rawPointer2);
        EXPECT_EQ(move.Get(), rawPointer1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }

    EXPECT_EQ(deleted2, rawPointer1);
}

TEST(SharedPtrTests, TemplatedMoveAssign)
{
    Derived* rawPointer1 = Memory::New<Derived>();
    Base* rawPointer2 = Memory::New<Base>();

    Base* deleted = nullptr;
    Derived* deleted2 = nullptr;

    {
        auto pointer = SharedPtr<Derived>(rawPointer1, MyDeleter<Derived>(&deleted2));
        auto move = SharedPtr<Base>(rawPointer2, MyDeleter<Base>(&deleted));

        move = std::move(pointer);

        EXPECT_EQ(deleted, rawPointer2);
        EXPECT_EQ(move.Get(), rawPointer1);
        EXPECT_EQ(pointer.GetCount(), 0);
    }

    EXPECT_EQ(deleted2, rawPointer1);
}

TEST(SharedPtrTests, ScopedPtrAssign)
{
    Derived* rawPointer1 = Memory::New<Derived>();
    Base* rawPointer2 = Memory::New<Base>();

    Base* deleted = nullptr;
    Derived* deleted2 = nullptr;

    {
        ScopedPtr<Derived, MyDeleter<Derived>> pointer(
            rawPointer1, MyDeleter<Derived>(&deleted2));

        auto move = SharedPtr<Base>(rawPointer2, MyDeleter<Base>(&deleted));
        move = std::move(pointer);

        EXPECT_EQ(deleted, rawPointer2);
        EXPECT_EQ(move.Get(), rawPointer1);
        EXPECT_EQ(pointer.Get(), nullptr);
    }

    EXPECT_EQ(deleted2, rawPointer1);
}

TEST(SharedPtrTests, Dereference)
{
    SharedPtr<int> pointer = MakeShared<int>(5);
    EXPECT_EQ(&*pointer, pointer.Get());
}

TEST(SharedPtrTests, Boolean)
{
    auto pointer = MakeShared<int>(5);
    auto empty = SharedPtr<int>();

    EXPECT_TRUE((bool)pointer);
    EXPECT_FALSE((bool)empty);
}

TEST(SharedPtrTests, GetCount)
{
    auto ptr1 = MakeShared<int>(2);
    auto pointer2 = ptr1;
    auto ptr3 = pointer2;
    auto ptr4 = ptr3;

    EXPECT_EQ(ptr1.GetCount(), 4);
}

TEST(SharedPtrTests, SwapMemberFunction)
{
    SharedPtr<int> pointer = MakeShared<int>(5);
    SharedPtr<int> pointer2 = MakeShared<int>(10);

    int* rawPointer = pointer.Get();
    int* rawPointer2 = pointer2.Get();

    Usize count = pointer.GetCount();
    Usize count2 = pointer2.GetCount();

    pointer.Swap(pointer2);

    EXPECT_EQ(pointer.Get(), rawPointer2);
    EXPECT_EQ(pointer2.Get(), rawPointer);

    EXPECT_EQ(pointer.GetCount(), count2);
    EXPECT_EQ(pointer2.GetCount(), count);
}

TEST(SharedPtrTests, Comparison)
{
    int* memory1 = Memory::New<int>();
    int* memory2 = Memory::New<int>();
    int* memory3 = Memory::New<int>();

    std::vector<SharedPtr<int>> array = {
        SharedPtr<int>(memory1),
        SharedPtr<int>(memory2),
        SharedPtr<int>(memory3)
    };

    for (auto& pointer1 : array)
    {
        for (auto& pointer2 : array)
        {
            EXPECT_EQ(pointer1 == pointer2,
                      pointer1.Get() == pointer2.Get());

            EXPECT_EQ(pointer1 != pointer2,
                      pointer1.Get() != pointer2.Get());

            EXPECT_EQ(pointer1 > pointer2,
                      pointer1.Get() > pointer2.Get());

            EXPECT_EQ(pointer1 < pointer2,
                      pointer1.Get() < pointer2.Get());

            EXPECT_EQ(pointer1 >= pointer2,
                      pointer1.Get() >= pointer2.Get());

            EXPECT_EQ(pointer1 <= pointer2,
                      pointer1.Get() <= pointer2.Get());
        }
    }

    int* null = nullptr;
    SharedPtr<int> pointer = MakeShared<int>(234);

    EXPECT_EQ(pointer == nullptr, pointer.Get() == null);
    EXPECT_EQ(pointer != nullptr, pointer.Get() != null);
    EXPECT_EQ(pointer < nullptr, pointer.Get() < null);
    EXPECT_EQ(pointer > nullptr, pointer.Get() > null);
    EXPECT_EQ(pointer <= nullptr, pointer.Get() <= null);
    EXPECT_EQ(pointer >= nullptr, pointer.Get() >= null);

    EXPECT_EQ(nullptr == pointer, null == pointer.Get());
    EXPECT_EQ(nullptr != pointer, null != pointer.Get());
    EXPECT_EQ(nullptr < pointer, null < pointer.Get());
    EXPECT_EQ(nullptr > pointer, null > pointer.Get());
    EXPECT_EQ(nullptr <= pointer, null <= pointer.Get());
    EXPECT_EQ(nullptr >= pointer, null >= pointer.Get());
}

TEST(WeakPtrTests, DefaultConstructor)
{
    WeakPtr<int> pointer;
    EXPECT_EQ(pointer.GetCount(), 0);
}

TEST(WeakPtrTests, SharedPtrConstructor)
{
    SharedPtr<int> null;
    SharedPtr<int> pointer = MakeShared<int>(5);

    WeakPtr<int> weakNull = null;
    WeakPtr<int> weak = pointer;

    EXPECT_EQ(weakNull.GetCount(), 0);
    EXPECT_EQ(weak.GetCount(), 1);
    EXPECT_EQ(weak.Lock(), pointer);
}

TEST(WeakPtrTests, CopyConstructor)
{
    SharedPtr<int> pointer = MakeShared<int>(5);

    WeakPtr<int> weak = pointer;
    WeakPtr<int> copy = weak;

    EXPECT_EQ(copy.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 1);

    EXPECT_EQ(weak.Lock(), pointer);
    EXPECT_EQ(copy.Lock(), pointer);
}

TEST(WeakPtrTests, TemplatedCopyConstructor)
{
    SharedPtr<Derived> pointer = MakeShared<Derived>();

    WeakPtr<Derived> weak = pointer;
    WeakPtr<Base> copy = weak;

    EXPECT_EQ(copy.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 1);

    EXPECT_EQ(weak.Lock(), pointer);
    EXPECT_EQ(copy.Lock(), pointer);
}

TEST(WeakPtrTests, MoveConstructor)
{
    SharedPtr<int> pointer = MakeShared<int>();

    WeakPtr<int> weak = pointer;
    WeakPtr<int> move = std::move(weak);

    EXPECT_EQ(move.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 0);

    EXPECT_EQ(move.Lock(), pointer);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, TemplatedMoveConstructor)
{
    SharedPtr<Derived> pointer = MakeShared<Derived>();

    WeakPtr<Derived> weak = pointer;
    WeakPtr<Base> move = std::move(weak);

    EXPECT_EQ(move.GetCount(), 1);
    EXPECT_EQ(weak.GetCount(), 0);

    EXPECT_EQ(move.Lock(), pointer);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, SharedPtrAssign)
{
    WeakPtr<Base> weak;

    {
        SharedPtr<Derived> pointer = MakeShared<Derived>();
        weak = pointer;

        EXPECT_EQ(weak.GetCount(), 1);
        EXPECT_EQ(weak.Lock(), pointer);
    }

    EXPECT_EQ(weak.GetCount(), 0);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, CopyAssign)
{
    SharedPtr<int> pointer = MakeShared<int>();
    SharedPtr<int> pointer2 = MakeShared<int>();

    WeakPtr<int> weak = pointer;
    WeakPtr<int> copy = pointer2;

    copy = weak;

    EXPECT_EQ(weak.GetCount(), 1);
    EXPECT_EQ(copy.GetCount(), 1);

    EXPECT_EQ(copy.Lock(), pointer);
}

TEST(WeakPtrTests, TemplatedCopyAssign)
{
    SharedPtr<Derived> pointer = MakeShared<Derived>();
    SharedPtr<Base> pointer2 = MakeShared<Base>();

    WeakPtr<Derived> weak = pointer;
    WeakPtr<Base> copy = pointer2;

    copy = weak;

    EXPECT_EQ(weak.GetCount(), 1);
    EXPECT_EQ(copy.GetCount(), 1);

    EXPECT_EQ(copy.Lock(), pointer);
}

TEST(WeakPtrTests, MoveAssign)
{
    SharedPtr<int> pointer = MakeShared<int>();
    SharedPtr<int> pointer2 = MakeShared<int>();

    WeakPtr<int> weak = pointer;
    WeakPtr<int> move = pointer2;

    move = std::move(weak);

    EXPECT_EQ(weak.GetCount(), 0);
    EXPECT_EQ(move.GetCount(), 1);

    EXPECT_EQ(move.Lock(), pointer);
    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, Reset)
{
    SharedPtr<int> pointer = MakeShared<int>();
    WeakPtr<int> weak = pointer;

    weak.Reset();

    EXPECT_EQ(weak.GetCount(), 0);
}

TEST(WeakPtrTests, GetCount)
{
    SharedPtr<int> pointer = MakeShared<int>();
    WeakPtr<int> weak = pointer;

    EXPECT_EQ(weak.GetCount(), 1);

    SharedPtr<int> copy = pointer;
    EXPECT_EQ(weak.GetCount(), 2);
}

TEST(WeakPtrTests, IsExpired)
{
    WeakPtr<Base> weak;
    WeakPtr<Base> null;

    {
        SharedPtr<Derived> pointer = MakeShared<Derived>();
        weak = pointer;
    }

    EXPECT_TRUE(weak.IsExpired());
    EXPECT_TRUE(null.IsExpired());
}

TEST(WeakPtrTests, Lock)
{
    WeakPtr<Base> weak;

    {
        SharedPtr<Derived> pointer = MakeShared<Derived>();
        weak = pointer;

        EXPECT_EQ(weak.Lock(), pointer);
    }

    EXPECT_EQ(weak.Lock(), nullptr);
}

TEST(WeakPtrTests, SwapMemberFn)
{
    SharedPtr<int> pointer = MakeShared<int>(5);
    SharedPtr<int> pointer2 = MakeShared<int>(10);

    WeakPtr<int> weakPtr = pointer;
    WeakPtr<int> weakPointer2 = pointer2;

    int* rawPointer = pointer.Get();
    int* rawPointer2 = pointer2.Get();

    weakPtr.Swap(weakPointer2);

    EXPECT_EQ(weakPtr.Lock().Get(), rawPointer2);
    EXPECT_EQ(weakPointer2.Lock().Get(), rawPointer);
}

TEST(WeakPtrTests, SwapAlgorithm)
{
    SharedPtr<int> pointer = MakeShared<int>(5);
    SharedPtr<int> pointer2 = MakeShared<int>(10);

    WeakPtr<int> weakPtr = pointer;
    WeakPtr<int> weakPointer2 = pointer2;

    int* rawPointer = pointer.Get();
    int* rawPointer2 = pointer2.Get();

    Swap(weakPtr, weakPointer2);

    EXPECT_EQ(weakPtr.Lock().Get(), rawPointer2);
    EXPECT_EQ(weakPointer2.Lock().Get(), rawPointer);
}

TEST(SharedPtrTests, StaticPointerCast)
{
    SharedPtr<void> voidPointer = SharedPtr<void>(Memory::New<int>(3));
    SharedPtr<int> intPointer = StaticPointerCast<int>(voidPointer);

    EXPECT_EQ((void*)intPointer.Get(), voidPointer.Get());

    EXPECT_EQ(intPointer.GetCount(), 2);
    EXPECT_EQ(voidPointer.GetCount(), 2);
}

TEST(SharedPtrTests, MoveStaticPointerCast)
{
    SharedPtr<void> voidPointer = SharedPtr<void>(Memory::New<int>(3));
    void* rawPointer = voidPointer.Get();

    SharedPtr<int> intPointer = StaticPointerCast<int>(Move(voidPointer));
    EXPECT_EQ((void*)intPointer.Get(), rawPointer);

    EXPECT_EQ(intPointer.GetCount(), 1);
    EXPECT_EQ(voidPointer.GetCount(), 0);
}

TEST(SharedPtrTests, DynamicPointerCast)
{
    SharedPtr<Base> pointer = MakeShared<Derived>();
    SharedPtr<Derived> intPointer = DynamicPointerCast<Derived>(pointer);

    EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

    EXPECT_EQ(intPointer.GetCount(), 2);
    EXPECT_EQ(pointer.GetCount(), 2);
}

TEST(SharedPtrTests, MoveDynamicPointerCast)
{
    SharedPtr<Base> pointer = MakeShared<Derived>();
    Base* rawPointer = pointer.Get();

    SharedPtr<Derived> intPointer = DynamicPointerCast<Derived>(Move(pointer));
    EXPECT_EQ((void*)intPointer.Get(), rawPointer);

    EXPECT_EQ(intPointer.GetCount(), 1);
    EXPECT_EQ(pointer.GetCount(), 0);
}

TEST(SharedPtrTests, ConstPointerCast)
{
    SharedPtr<const int> pointer = MakeShared<int>();
    SharedPtr<int> intPointer = ConstPointerCast<int>(pointer);

    EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

    EXPECT_EQ(intPointer.GetCount(), 2);
    EXPECT_EQ(pointer.GetCount(), 2);
}

TEST(SharedPtrTests, MoveConstPointerCast)
{
    SharedPtr<const int> pointer = MakeShared<int>();
    const int* rawPointer = pointer.Get();

    SharedPtr<int> intPointer = ConstPointerCast<int>(Move(pointer));
    EXPECT_EQ((void*)intPointer.Get(), rawPointer);

    EXPECT_EQ(intPointer.GetCount(), 1);
    EXPECT_EQ(pointer.GetCount(), 0);
}

TEST(SharedPtrTests, ReinterpretPointerCast)
{
    SharedPtr<int> pointer = MakeShared<int>();
    SharedPtr<float> intPointer = ReinterpretPointerCast<float>(pointer);

    EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

    EXPECT_EQ(intPointer.GetCount(), 2);
    EXPECT_EQ(pointer.GetCount(), 2);
}

TEST(SharedPtrTests, MoveReinterpretPointerCast)
{
    SharedPtr<int> pointer = MakeShared<int>();
    int* rawPointer = pointer.Get();

    SharedPtr<float> intPointer = ReinterpretPointerCast<float>(Move(pointer));
    EXPECT_EQ((void*)intPointer.Get(), rawPointer);

    EXPECT_EQ(intPointer.GetCount(), 1);
    EXPECT_EQ(pointer.GetCount(), 0);
}
