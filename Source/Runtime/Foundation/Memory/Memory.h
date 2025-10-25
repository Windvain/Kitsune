#pragma once

#include <new>
#include <type_traits>

#include "Foundation/Common/Types.h"
#include "Foundation/Templates/Forward.h"

#include "Foundation/Memory/IMemoryApi.h"
#include "Foundation/Memory/MemoryProtection.h"

namespace Kitsune
{
    class Memory
    {
    public:
        static bool InitializeExplicit();
        static void Shutdown();

    public:
        [[nodiscard]] static void* TryAllocate(Usize bytes);
        [[nodiscard]] static void* TryAllocate(Usize bytes, Usize alignment);

        [[nodiscard]] static void* Allocate(Usize bytes);
        [[nodiscard]] static void* Allocate(Usize bytes, Usize alignment);

        static void Free(void* ptr);

    public:
        [[nodiscard]] static void* VirtualAllocate(Usize bytes, MemoryProtection protection);
        static void VirtualFree(void* ptr, Usize bytes);

    public:
        [[nodiscard]]
        static inline Usize GetDefaultAlignment()
        {
            return s_MemoryApi->GetDefaultAlignment();
        }

    public:
        template<typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        static T* ConstructAt(T* ptr, Args&&... args)
        {
            new (static_cast<void*>(ptr)) T(Forward<Args>(args)...);
            return ptr;
        }

        template<typename T>
        static void DestroyAt(T* ptr)
        {
            ptr->~T();
        }

        template<typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        [[nodiscard]] static T* New(Args&&... args)
        {
            T* ptr = (T*)Memory::Allocate(sizeof(T), alignof(T));
            return ConstructAt(ptr, Forward<Args>(args)...);
        }

        template<typename T>
        static void Delete(T* ptr)
        {
            DestroyAt(ptr);
            Memory::Free(ptr);
        }

    private:
        static bool s_Initialized;
        static IMemoryApi* s_MemoryApi;
    };
}
