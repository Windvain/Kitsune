// TODO: Uncomment this after reimplementing SharedPtr<T>.
// #include <gtest/gtest.h>
// #include "Foundation/Memory/SharedPtr.h"

// namespace
// {
//     using namespace Kitsune;

//     class TrackingAllocator
//     {
//     public:
//         TrackingAllocator() = default;

//         // We check the allocator's state externally, so the allocations have to be
//         // copied.
//         TrackingAllocator(const TrackingAllocator& allocator) = default;
//         TrackingAllocator(TrackingAllocator&& allocator) = default;

//         ~TrackingAllocator() = default;

//     public:
//         TrackingAllocator& operator=(const TrackingAllocator& allocator)
//         {
//             return *this;
//         }

//         TrackingAllocator& operator=(TrackingAllocator&& allocator)
//         {
//             m_Allocations = std::move(allocator.m_Allocations);
//             return *this;
//         }

//     public:
//         inline void* Allocate(
//             Usize bytes,
//             Usize alignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__)
//         {
//             void* pointer = Memory::Allocate(bytes, alignment);
//             m_Allocations.insert({ pointer, bytes });

//             return pointer;
//         }

//         inline void Free(void* pointer, Usize bytes)
//         {
//             EXPECT_TRUE(m_Allocations.contains(pointer));
//             EXPECT_EQ(m_Allocations[pointer], bytes);

//             m_Allocations.erase(pointer);
//             Memory::Free(pointer, bytes);
//         }

//     public:
//         inline bool operator==(const TrackingAllocator& otherAlloc) const
//         {
//             return (this == &otherAlloc);
//         }

//     private:
//         std::unordered_map<void*, std::size_t> m_Allocations;
//     };

//     template<typename T>
//     class TrackingDeleter
//     {
//     public:
//         using ValueType = T;
//         using StorageType = std::vector<void*>;

//         inline TrackingDeleter() = default;
//         inline TrackingDeleter(StorageType* storage)
//             : m_Storage(storage)
//         {
//         }

//         inline TrackingDeleter(const TrackingDeleter&) = default;
//         inline TrackingDeleter(TrackingDeleter&& deleter)
//             : m_Storage(std::exchange(deleter.m_Storage, nullptr))
//         {
//         }

//         ~TrackingDeleter() = default;

//     public:
//         inline TrackingDeleter& operator=(const TrackingDeleter& deleter)
//         {
//             if (this == &deleter)
//                 return *this;

//             KITSUNE_UNREACHABLE();
//             return *this;
//         }

//         inline TrackingDeleter& operator=(TrackingDeleter&& deleter)
//         {
//             if (this == &deleter)
//                 return *this;

//             KITSUNE_UNREACHABLE();
//             return *this;
//         }

//     public:
//         inline void operator()(T* pointer)
//         {
//             if ((m_Storage == nullptr) || (pointer == nullptr))
//                 return;

//             m_Storage->push_back(pointer);
//             Memory::Delete(pointer);
//         }

//     private:
//         template<typename U>
//         friend class MyDeleter;

//         StorageType* m_Storage = nullptr;
//     };

//     static_assert(
//         Deleter<TrackingDeleter<int>>,
//         "TrackingDeleter<T> does not fulfill the requirements for Deleter.");

//     class Base
//     {
//     public:
//         virtual ~Base() = default;
//     };

//     class Derived : public Base
//     {
//     };

//     TEST(SharedPtrTest, DefaultNullptrConstructor)
//     {
//         SharedPtr<int> pointer;
//         SharedPtr<int> null = nullptr;

//         EXPECT_EQ(pointer.Get(), nullptr);
//         EXPECT_EQ(null.Get(), nullptr);

//         EXPECT_EQ(pointer.GetCount(), 0);
//         EXPECT_EQ(null.GetCount(), 0);
//     }

//     TEST(SharedPtrTest, PointerConstructor)
//     {
//         int* rawPointer = Memory::New<int>(234);
//         auto pointer = SharedPtr<int>(rawPointer);

//         EXPECT_EQ(pointer.Get(), rawPointer);
//         EXPECT_EQ(pointer.GetCount(), 1);

//         // rawPointer here should've been destroyed by GlobalAllocator.
//     }

//     TEST(SharedPtrTest, PointerDeleterConstructor)
//     {
//         int* rawPointer1 = Memory::New<int>();
//         int* rawPointer2 = Memory::New<int>();

//         std::vector<void*> deleted;

//         {
//             TrackingDeleter<int> deleter(&deleted);

//             SharedPtr<int> moved(rawPointer1, TrackingDeleter<int>(&deleted));
//             SharedPtr<int> copied(rawPointer2, deleter);

//             EXPECT_EQ(moved.Get(), rawPointer1);
//             EXPECT_EQ(moved.GetCount(), 1);

//             EXPECT_EQ(copied.Get(), rawPointer2);
//             EXPECT_EQ(copied.GetCount(), 1);
//         }

//         EXPECT_EQ(deleted[0], rawPointer2);
//         EXPECT_EQ(deleted[1], rawPointer1);
//     }

//     TEST(SharedPtrTest, NullptrDeleterConstructor)
//     {
//         std::vector<void*> deleted;

//         {
//             TrackingDeleter<int> deleter(&deleted);

//             SharedPtr<int> moved(nullptr, TrackingDeleter<int>(&deleted));
//             SharedPtr<int> copied(nullptr, deleter);

//             EXPECT_EQ(moved.Get(), nullptr);
//             EXPECT_EQ(moved.GetCount(), 1);

//             EXPECT_EQ(copied.Get(), nullptr);
//             EXPECT_EQ(copied.GetCount(), 1);
//         }

//         EXPECT_EQ(deleted.size(), 0);
//     }

//     TEST(SharedPtrTest, CopyDeleterAndAllocatorConstructor)
//     {
//         int* rawPointer = Memory::New<int>();
//         std::vector<void*> deleted;

//         {
//             TrackingDeleter<int> deleter(&deleted);
//             SharedPtr<int> pointer(rawPointer, deleter, TrackingAllocator());

//             EXPECT_EQ(pointer.Get(), rawPointer);
//             EXPECT_EQ(pointer.GetCount(), 1);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, MoveDeleterAndAllocatorConstructor)
//     {
//         int* rawPointer = Memory::New<int>();
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> pointer(
//                 rawPointer,
//                 TrackingDeleter<int>(&deleted),
//                 TrackingAllocator());

//             EXPECT_EQ(pointer.Get(), rawPointer);
//             EXPECT_EQ(pointer.GetCount(), 1);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, NullptrCopyDeleterAndAllocatorConstructor)
//     {
//         std::vector<void*> deleted;

//         {
//             TrackingDeleter<int> deleter(&deleted);
//             SharedPtr<int> pointer(nullptr, deleter, TrackingAllocator());

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 1);
//         }

//         EXPECT_EQ(deleted.size(), 0);
//     }

//     TEST(SharedPtrTest, NullptrMoveDeleterAndAllocatorConstructor)
//     {
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> pointer(
//                 nullptr,
//                 TrackingDeleter<int>(&deleted),
//                 TrackingAllocator());

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 1);
//         }

//         EXPECT_EQ(deleted.size(), 0);
//     }

//     TEST(SharedPtrTest, CopyConstructor)
//     {
//         int* rawPointer = Memory::New<int>(4);
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));

//             // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
//             SharedPtr<int> copy = pointer;

//             EXPECT_EQ(copy.Get(), rawPointer);
//             EXPECT_EQ(copy.GetCount(), 2);

//             EXPECT_EQ(pointer.Get(), rawPointer);
//             EXPECT_EQ(pointer.GetCount(), 2);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, CopyConstructorHandlesNullptr)
//     {
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> pointer(nullptr, TrackingDeleter<int>(&deleted));

//             // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
//             SharedPtr<int> copy = pointer;

//             EXPECT_EQ(copy.Get(), nullptr);
//             EXPECT_EQ(copy.GetCount(), 0);

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 0);
//         }

//         EXPECT_EQ(deleted.size(), 0);
//     }

//     TEST(SharedPtrTest, AliasingCopyConstructor)
//     {
//         int* rawPointer = Memory::New<int>(45);
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> ownedPointer(rawPointer, TrackingDeleter<int>(&deleted));

//             int* storedPointer = reinterpret_cast<int*>(0xDEADC0DE);
//             SharedPtr<int> pointer(ownedPointer, storedPointer);

//             EXPECT_EQ(pointer.Get(), storedPointer);
//             EXPECT_EQ(pointer.GetCount(), 2);

//             EXPECT_EQ(ownedPointer.Get(), rawPointer);
//             EXPECT_EQ(ownedPointer.GetCount(), 2);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, TemplatedCopyConstructor)
//     {
//         auto* rawPointer = Memory::New<Derived>();
//         std::vector<void*> deleted;

//         {
//             SharedPtr<Derived> pointer(rawPointer, TrackingDeleter<Derived>(&deleted));

//             // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
//             SharedPtr<Base> copy = pointer;

//             EXPECT_EQ(copy.Get(), rawPointer);
//             EXPECT_EQ(copy.GetCount(), 2);

//             EXPECT_EQ(pointer.Get(), rawPointer);
//             EXPECT_EQ(pointer.GetCount(), 2);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, TemplatedCopyConstructorHandlesNullptr)
//     {
//         std::vector<void*> deleted;

//         {
//             SharedPtr<Derived> pointer(nullptr, TrackingDeleter<Derived>(&deleted));

//             // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
//             SharedPtr<Base> copy = pointer;

//             EXPECT_EQ(copy.Get(), nullptr);
//             EXPECT_EQ(copy.GetCount(), 0);

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 0);
//         }

//         EXPECT_EQ(deleted.size(), 0);
//     }

//     TEST(SharedPtrTest, MoveConstructor)
//     {
//         std::vector<void*> deleted;
//         int* rawPointer = Memory::New<int>(5);

//         {
//             SharedPtr<int> pointer(rawPointer);
//             SharedPtr<int> moved = std::move(pointer);

//             EXPECT_EQ(moved.Get(), rawPointer);
//             EXPECT_EQ(moved.GetCount(), 1);

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 0);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, MoveConstructorHandlesNullptr)
//     {
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> pointer;
//             SharedPtr<int> moved = std::move(pointer);

//             EXPECT_EQ(moved.Get(), nullptr);
//             EXPECT_EQ(moved.GetCount(), 0);

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 0);
//         }

//         EXPECT_EQ(deleted.size(), 0);
//     }

//     TEST(SharedPtrTest, TemplatedMoveConstructor)
//     {
//         std::vector<void*> deleted;
//         auto* rawPointer = Memory::New<Derived>();

//         {
//             SharedPtr<Derived> pointer(rawPointer);
//             SharedPtr<Base> moved = std::move(pointer);

//             EXPECT_EQ(moved.Get(), rawPointer);
//             EXPECT_EQ(moved.GetCount(), 1);

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 0);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, TemplatedMoveConstructorHandlesNullptr)
//     {
//         std::vector<void*> deleted;

//         {
//             SharedPtr<Derived> pointer;
//             SharedPtr<Base> moved = std::move(pointer);

//             EXPECT_EQ(moved.Get(), nullptr);
//             EXPECT_EQ(moved.GetCount(), 0);

//             EXPECT_EQ(pointer.Get(), nullptr);
//             EXPECT_EQ(pointer.GetCount(), 0);
//         }

//         EXPECT_EQ(deleted.size(), 0);
//     }

//     TEST(SharedPtrTest, AliasingMoveConstructor)
//     {
//         int* rawPointer = Memory::New<int>(45);
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> ownedPointer(rawPointer, TrackingDeleter<int>(&deleted));

//             int* storedPointer = reinterpret_cast<int*>(0xDEADC0DE);
//             SharedPtr<int> pointer(std::move(ownedPointer), storedPointer);

//             EXPECT_EQ(pointer.Get(), storedPointer);
//             EXPECT_EQ(pointer.GetCount(), 1);

//             EXPECT_EQ(ownedPointer.Get(), nullptr);
//             EXPECT_EQ(ownedPointer.GetCount(), 0);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, WeakPtrConstructor)
//     {
//         int* rawPointer = Memory::New<int>(45);
//         std::vector<void*> deleted;

//         {
//             SharedPtr<int> pointer(rawPointer, TrackingDeleter<int>(&deleted));
//             WeakPtr<int> weakPointer(pointer);

//             ASSERT_EQ(pointer.GetCount(), 1);
//             ASSERT_EQ(pointer.Get(), rawPointer);

//             SharedPtr<int> copy(weakPointer);
//             EXPECT_EQ(copy.Get(), rawPointer);
//             EXPECT_EQ(copy.GetCount(), 2);

//             EXPECT_EQ(pointer.Get(), rawPointer);
//             EXPECT_EQ(pointer.GetCount(), 2);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }

//     TEST(SharedPtrTest, ScopedPtrConstructor)
//     {
//         auto* rawPointer = Memory::New<Derived>();
//         std::vector<void*> deleted;

//         {
//             ScopedPtr<Derived, TrackingDeleter<Derived>> pointer(
//                 rawPointer,
//                 TrackingDeleter<Derived>(&deleted));

//             SharedPtr<Base> moved(std::move(pointer));
//             EXPECT_EQ(moved.Get(), rawPointer);
//             EXPECT_EQ(moved.GetCount(), 1);

//             EXPECT_EQ(pointer.Get(), nullptr);
//         }

//         EXPECT_EQ(deleted.size(), 1);
//         EXPECT_EQ(deleted[0], rawPointer);
//     }
// }















// // TEST(SharedPtrTest, Destructor)
// // {
// //     int* rawPointer = Memory::New<int>(5);
// //     int* deleted = nullptr;

// //     {
// //         TrackingDeleter<int> deleter = TrackingDeleter<int>(&deleted);
// //         SharedPtr<int> pointer = SharedPtr<int>(rawPointer, std::move(deleter));

// //         KITSUNE_UNUSED(pointer);
// //     }

// //     EXPECT_EQ(rawPointer, deleted);
// // }

// // TEST(SharedPtrTest, CopyAssign)
// // {
// //     int* rawPointer1 = Memory::New<int>(5);
// //     int* rawPointer2 = Memory::New<int>(10);

// //     int* deleted = nullptr;
// //     int* deleted2 = nullptr;

// //     {
// //         auto pointer = SharedPtr<int>(rawPointer1, TrackingDeleter<int>(&deleted2));
// //         auto copy = SharedPtr<int>(rawPointer2, TrackingDeleter<int>(&deleted));

// //         copy = pointer;

// //         EXPECT_EQ(deleted, rawPointer2);
// //         EXPECT_EQ(copy.Get(), pointer.Get());
// //     }

// //     EXPECT_EQ(deleted2, rawPointer1);
// // }

// // TEST(SharedPtrTest, TemplateCopyAssign)
// // {
// //     Derived* rawPointer1 = Memory::New<Derived>();
// //     Base* rawPointer2 = Memory::New<Base>();

// //     Base* deleted = nullptr;
// //     Derived* deleted2 = nullptr;

// //     {
// //         auto pointer = SharedPtr<Derived>(rawPointer1, TrackingDeleter<Derived>(&deleted2));
// //         auto copy = SharedPtr<Base>(rawPointer2, TrackingDeleter<Base>(&deleted));

// //         copy = pointer;

// //         EXPECT_EQ(deleted, rawPointer2);
// //         EXPECT_EQ(copy.Get(), pointer.Get());
// //     }

// //     EXPECT_EQ(deleted2, rawPointer1);
// // }

// // TEST(SharedPtrTest, MoveAssign)
// // {
// //     int* rawPointer1 = Memory::New<int>(5);
// //     int* rawPointer2 = Memory::New<int>(10);

// //     int* deleted = nullptr;
// //     int* deleted2 = nullptr;

// //     {
// //         auto pointer = SharedPtr<int>(rawPointer1, TrackingDeleter<int>(&deleted2));
// //         auto move = SharedPtr<int>(rawPointer2, TrackingDeleter<int>(&deleted));

// //         move = std::move(pointer);

// //         EXPECT_EQ(deleted, rawPointer2);
// //         EXPECT_EQ(move.Get(), rawPointer1);
// //         EXPECT_EQ(pointer.GetCount(), 0);
// //     }

// //     EXPECT_EQ(deleted2, rawPointer1);
// // }

// // TEST(SharedPtrTest, TemplatedMoveAssign)
// // {
// //     Derived* rawPointer1 = Memory::New<Derived>();
// //     Base* rawPointer2 = Memory::New<Base>();

// //     Base* deleted = nullptr;
// //     Derived* deleted2 = nullptr;

// //     {
// //         auto pointer = SharedPtr<Derived>(rawPointer1, TrackingDeleter<Derived>(&deleted2));
// //         auto move = SharedPtr<Base>(rawPointer2, TrackingDeleter<Base>(&deleted));

// //         move = std::move(pointer);

// //         EXPECT_EQ(deleted, rawPointer2);
// //         EXPECT_EQ(move.Get(), rawPointer1);
// //         EXPECT_EQ(pointer.GetCount(), 0);
// //     }

// //     EXPECT_EQ(deleted2, rawPointer1);
// // }

// // TEST(SharedPtrTest, ScopedPtrAssign)
// // {
// //     Derived* rawPointer1 = Memory::New<Derived>();
// //     Base* rawPointer2 = Memory::New<Base>();

// //     Base* deleted = nullptr;
// //     Derived* deleted2 = nullptr;

// //     {
// //         ScopedPtr<Derived, TrackingDeleter<Derived>> pointer(
// //             rawPointer1, TrackingDeleter<Derived>(&deleted2));

// //         auto move = SharedPtr<Base>(rawPointer2, TrackingDeleter<Base>(&deleted));
// //         move = std::move(pointer);

// //         EXPECT_EQ(deleted, rawPointer2);
// //         EXPECT_EQ(move.Get(), rawPointer1);
// //         EXPECT_EQ(pointer.Get(), nullptr);
// //     }

// //     EXPECT_EQ(deleted2, rawPointer1);
// // }

// // TEST(SharedPtrTest, Dereference)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>(5);
// //     EXPECT_EQ(&*pointer, pointer.Get());
// // }

// // TEST(SharedPtrTest, Boolean)
// // {
// //     auto pointer = MakeShared<int>(5);
// //     auto empty = SharedPtr<int>();

// //     EXPECT_TRUE((bool)pointer);
// //     EXPECT_FALSE((bool)empty);
// // }

// // TEST(SharedPtrTest, GetCount)
// // {
// //     auto ptr1 = MakeShared<int>(2);
// //     auto pointer2 = ptr1;
// //     auto ptr3 = pointer2;
// //     auto ptr4 = ptr3;

// //     EXPECT_EQ(ptr1.GetCount(), 4);
// // }

// // TEST(SharedPtrTest, SwapMemberFunction)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>(5);
// //     SharedPtr<int> pointer2 = MakeShared<int>(10);

// //     int* rawPointer = pointer.Get();
// //     int* rawPointer2 = pointer2.Get();

// //     Usize count = pointer.GetCount();
// //     Usize count2 = pointer2.GetCount();

// //     pointer.Swap(pointer2);

// //     EXPECT_EQ(pointer.Get(), rawPointer2);
// //     EXPECT_EQ(pointer2.Get(), rawPointer);

// //     EXPECT_EQ(pointer.GetCount(), count2);
// //     EXPECT_EQ(pointer2.GetCount(), count);
// // }

// // TEST(SharedPtrTest, Comparison)
// // {
// //     int* memory1 = Memory::New<int>();
// //     int* memory2 = Memory::New<int>();
// //     int* memory3 = Memory::New<int>();

// //     std::vector<SharedPtr<int>> array = {
// //         SharedPtr<int>(memory1),
// //         SharedPtr<int>(memory2),
// //         SharedPtr<int>(memory3)
// //     };

// //     for (auto& pointer1 : array)
// //     {
// //         for (auto& pointer2 : array)
// //         {
// //             EXPECT_EQ(pointer1 == pointer2,
// //                       pointer1.Get() == pointer2.Get());

// //             EXPECT_EQ(pointer1 != pointer2,
// //                       pointer1.Get() != pointer2.Get());

// //             EXPECT_EQ(pointer1 > pointer2,
// //                       pointer1.Get() > pointer2.Get());

// //             EXPECT_EQ(pointer1 < pointer2,
// //                       pointer1.Get() < pointer2.Get());

// //             EXPECT_EQ(pointer1 >= pointer2,
// //                       pointer1.Get() >= pointer2.Get());

// //             EXPECT_EQ(pointer1 <= pointer2,
// //                       pointer1.Get() <= pointer2.Get());
// //         }
// //     }

// //     int* null = nullptr;
// //     SharedPtr<int> pointer = MakeShared<int>(234);

// //     EXPECT_EQ(pointer == nullptr, pointer.Get() == null);
// //     EXPECT_EQ(pointer != nullptr, pointer.Get() != null);
// //     EXPECT_EQ(pointer < nullptr, pointer.Get() < null);
// //     EXPECT_EQ(pointer > nullptr, pointer.Get() > null);
// //     EXPECT_EQ(pointer <= nullptr, pointer.Get() <= null);
// //     EXPECT_EQ(pointer >= nullptr, pointer.Get() >= null);

// //     EXPECT_EQ(nullptr == pointer, null == pointer.Get());
// //     EXPECT_EQ(nullptr != pointer, null != pointer.Get());
// //     EXPECT_EQ(nullptr < pointer, null < pointer.Get());
// //     EXPECT_EQ(nullptr > pointer, null > pointer.Get());
// //     EXPECT_EQ(nullptr <= pointer, null <= pointer.Get());
// //     EXPECT_EQ(nullptr >= pointer, null >= pointer.Get());
// // }

// // TEST(WeakPtrTests, DefaultConstructor)
// // {
// //     WeakPtr<int> pointer;
// //     EXPECT_EQ(pointer.GetCount(), 0);
// // }

// // TEST(WeakPtrTests, SharedPtrConstructor)
// // {
// //     SharedPtr<int> null;
// //     SharedPtr<int> pointer = MakeShared<int>(5);

// //     WeakPtr<int> weakNull = null;
// //     WeakPtr<int> weak = pointer;

// //     EXPECT_EQ(weakNull.GetCount(), 0);
// //     EXPECT_EQ(weak.GetCount(), 1);
// //     EXPECT_EQ(weak.Lock(), pointer);
// // }

// // TEST(WeakPtrTests, CopyConstructor)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>(5);

// //     WeakPtr<int> weak = pointer;
// //     WeakPtr<int> copy = weak;

// //     EXPECT_EQ(copy.GetCount(), 1);
// //     EXPECT_EQ(weak.GetCount(), 1);

// //     EXPECT_EQ(weak.Lock(), pointer);
// //     EXPECT_EQ(copy.Lock(), pointer);
// // }

// // TEST(WeakPtrTests, TemplatedCopyConstructor)
// // {
// //     SharedPtr<Derived> pointer = MakeShared<Derived>();

// //     WeakPtr<Derived> weak = pointer;
// //     WeakPtr<Base> copy = weak;

// //     EXPECT_EQ(copy.GetCount(), 1);
// //     EXPECT_EQ(weak.GetCount(), 1);

// //     EXPECT_EQ(weak.Lock(), pointer);
// //     EXPECT_EQ(copy.Lock(), pointer);
// // }

// // TEST(WeakPtrTests, MoveConstructor)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>();

// //     WeakPtr<int> weak = pointer;
// //     WeakPtr<int> move = std::move(weak);

// //     EXPECT_EQ(move.GetCount(), 1);
// //     EXPECT_EQ(weak.GetCount(), 0);

// //     EXPECT_EQ(move.Lock(), pointer);
// //     EXPECT_EQ(weak.Lock(), nullptr);
// // }

// // TEST(WeakPtrTests, TemplatedMoveConstructor)
// // {
// //     SharedPtr<Derived> pointer = MakeShared<Derived>();

// //     WeakPtr<Derived> weak = pointer;
// //     WeakPtr<Base> move = std::move(weak);

// //     EXPECT_EQ(move.GetCount(), 1);
// //     EXPECT_EQ(weak.GetCount(), 0);

// //     EXPECT_EQ(move.Lock(), pointer);
// //     EXPECT_EQ(weak.Lock(), nullptr);
// // }

// // TEST(WeakPtrTests, SharedPtrAssign)
// // {
// //     WeakPtr<Base> weak;

// //     {
// //         SharedPtr<Derived> pointer = MakeShared<Derived>();
// //         weak = pointer;

// //         EXPECT_EQ(weak.GetCount(), 1);
// //         EXPECT_EQ(weak.Lock(), pointer);
// //     }

// //     EXPECT_EQ(weak.GetCount(), 0);
// //     EXPECT_EQ(weak.Lock(), nullptr);
// // }

// // TEST(WeakPtrTests, CopyAssign)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>();
// //     SharedPtr<int> pointer2 = MakeShared<int>();

// //     WeakPtr<int> weak = pointer;
// //     WeakPtr<int> copy = pointer2;

// //     copy = weak;

// //     EXPECT_EQ(weak.GetCount(), 1);
// //     EXPECT_EQ(copy.GetCount(), 1);

// //     EXPECT_EQ(copy.Lock(), pointer);
// // }

// // TEST(WeakPtrTests, TemplatedCopyAssign)
// // {
// //     SharedPtr<Derived> pointer = MakeShared<Derived>();
// //     SharedPtr<Base> pointer2 = MakeShared<Base>();

// //     WeakPtr<Derived> weak = pointer;
// //     WeakPtr<Base> copy = pointer2;

// //     copy = weak;

// //     EXPECT_EQ(weak.GetCount(), 1);
// //     EXPECT_EQ(copy.GetCount(), 1);

// //     EXPECT_EQ(copy.Lock(), pointer);
// // }

// // TEST(WeakPtrTests, MoveAssign)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>();
// //     SharedPtr<int> pointer2 = MakeShared<int>();

// //     WeakPtr<int> weak = pointer;
// //     WeakPtr<int> move = pointer2;

// //     move = std::move(weak);

// //     EXPECT_EQ(weak.GetCount(), 0);
// //     EXPECT_EQ(move.GetCount(), 1);

// //     EXPECT_EQ(move.Lock(), pointer);
// //     EXPECT_EQ(weak.Lock(), nullptr);
// // }

// // TEST(WeakPtrTests, Reset)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>();
// //     WeakPtr<int> weak = pointer;

// //     weak.Reset();

// //     EXPECT_EQ(weak.GetCount(), 0);
// // }

// // TEST(WeakPtrTests, GetCount)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>();
// //     WeakPtr<int> weak = pointer;

// //     EXPECT_EQ(weak.GetCount(), 1);

// //     SharedPtr<int> copy = pointer;
// //     EXPECT_EQ(weak.GetCount(), 2);
// // }

// // TEST(WeakPtrTests, IsExpired)
// // {
// //     WeakPtr<Base> weak;
// //     WeakPtr<Base> null;

// //     {
// //         SharedPtr<Derived> pointer = MakeShared<Derived>();
// //         weak = pointer;
// //     }

// //     EXPECT_TRUE(weak.IsExpired());
// //     EXPECT_TRUE(null.IsExpired());
// // }

// // TEST(WeakPtrTests, Lock)
// // {
// //     WeakPtr<Base> weak;

// //     {
// //         SharedPtr<Derived> pointer = MakeShared<Derived>();
// //         weak = pointer;

// //         EXPECT_EQ(weak.Lock(), pointer);
// //     }

// //     EXPECT_EQ(weak.Lock(), nullptr);
// // }

// // TEST(WeakPtrTests, SwapMemberFn)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>(5);
// //     SharedPtr<int> pointer2 = MakeShared<int>(10);

// //     WeakPtr<int> weakPtr = pointer;
// //     WeakPtr<int> weakPointer2 = pointer2;

// //     int* rawPointer = pointer.Get();
// //     int* rawPointer2 = pointer2.Get();

// //     weakPtr.Swap(weakPointer2);

// //     EXPECT_EQ(weakPtr.Lock().Get(), rawPointer2);
// //     EXPECT_EQ(weakPointer2.Lock().Get(), rawPointer);
// // }

// // TEST(WeakPtrTests, SwapAlgorithm)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>(5);
// //     SharedPtr<int> pointer2 = MakeShared<int>(10);

// //     WeakPtr<int> weakPtr = pointer;
// //     WeakPtr<int> weakPointer2 = pointer2;

// //     int* rawPointer = pointer.Get();
// //     int* rawPointer2 = pointer2.Get();

// //     Swap(weakPtr, weakPointer2);

// //     EXPECT_EQ(weakPtr.Lock().Get(), rawPointer2);
// //     EXPECT_EQ(weakPointer2.Lock().Get(), rawPointer);
// // }

// // TEST(SharedPtrTest, StaticPointerCast)
// // {
// //     SharedPtr<void> voidPointer = SharedPtr<void>(Memory::New<int>(3));
// //     SharedPtr<int> intPointer = StaticPointerCast<int>(voidPointer);

// //     EXPECT_EQ((void*)intPointer.Get(), voidPointer.Get());

// //     EXPECT_EQ(intPointer.GetCount(), 2);
// //     EXPECT_EQ(voidPointer.GetCount(), 2);
// // }

// // TEST(SharedPtrTest, MoveStaticPointerCast)
// // {
// //     SharedPtr<void> voidPointer = SharedPtr<void>(Memory::New<int>(3));
// //     void* rawPointer = voidPointer.Get();

// //     SharedPtr<int> intPointer = StaticPointerCast<int>(Move(voidPointer));
// //     EXPECT_EQ((void*)intPointer.Get(), rawPointer);

// //     EXPECT_EQ(intPointer.GetCount(), 1);
// //     EXPECT_EQ(voidPointer.GetCount(), 0);
// // }

// // TEST(SharedPtrTest, DynamicPointerCast)
// // {
// //     SharedPtr<Base> pointer = MakeShared<Derived>();
// //     SharedPtr<Derived> intPointer = DynamicPointerCast<Derived>(pointer);

// //     EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

// //     EXPECT_EQ(intPointer.GetCount(), 2);
// //     EXPECT_EQ(pointer.GetCount(), 2);
// // }

// // TEST(SharedPtrTest, MoveDynamicPointerCast)
// // {
// //     SharedPtr<Base> pointer = MakeShared<Derived>();
// //     Base* rawPointer = pointer.Get();

// //     SharedPtr<Derived> intPointer = DynamicPointerCast<Derived>(Move(pointer));
// //     EXPECT_EQ((void*)intPointer.Get(), rawPointer);

// //     EXPECT_EQ(intPointer.GetCount(), 1);
// //     EXPECT_EQ(pointer.GetCount(), 0);
// // }

// // TEST(SharedPtrTest, ConstPointerCast)
// // {
// //     SharedPtr<const int> pointer = MakeShared<int>();
// //     SharedPtr<int> intPointer = ConstPointerCast<int>(pointer);

// //     EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

// //     EXPECT_EQ(intPointer.GetCount(), 2);
// //     EXPECT_EQ(pointer.GetCount(), 2);
// // }

// // TEST(SharedPtrTest, MoveConstPointerCast)
// // {
// //     SharedPtr<const int> pointer = MakeShared<int>();
// //     const int* rawPointer = pointer.Get();

// //     SharedPtr<int> intPointer = ConstPointerCast<int>(Move(pointer));
// //     EXPECT_EQ((void*)intPointer.Get(), rawPointer);

// //     EXPECT_EQ(intPointer.GetCount(), 1);
// //     EXPECT_EQ(pointer.GetCount(), 0);
// // }

// // TEST(SharedPtrTest, ReinterpretPointerCast)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>();
// //     SharedPtr<float> intPointer = ReinterpretPointerCast<float>(pointer);

// //     EXPECT_EQ((void*)intPointer.Get(), pointer.Get());

// //     EXPECT_EQ(intPointer.GetCount(), 2);
// //     EXPECT_EQ(pointer.GetCount(), 2);
// // }

// // TEST(SharedPtrTest, MoveReinterpretPointerCast)
// // {
// //     SharedPtr<int> pointer = MakeShared<int>();
// //     int* rawPointer = pointer.Get();

// //     SharedPtr<float> intPointer = ReinterpretPointerCast<float>(Move(pointer));
// //     EXPECT_EQ((void*)intPointer.Get(), rawPointer);

// //     EXPECT_EQ(intPointer.GetCount(), 1);
// //     EXPECT_EQ(pointer.GetCount(), 0);
// // }
