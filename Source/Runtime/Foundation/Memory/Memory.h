#pragma once

#include <new>
#include <type_traits>

#include "Foundation/Common/Types.h"
#include "Foundation/Common/Macros.h"
#include "Foundation/Templates/Forward.h"

#include "Foundation/Memory/IMemoryApi.h"

namespace Kitsune
{
    class Memory
    {
    public:
        KITSUNE_API_ static void InitializeExplicit();
        KITSUNE_API_ static void Shutdown();

    public:
        [[nodiscard]] KITSUNE_API_ static void* TryAllocate(Usize bytes);
        [[nodiscard]] KITSUNE_API_ static void* TryAllocate(Usize bytes, Usize alignment);

        [[nodiscard]] KITSUNE_API_ static void* Allocate(Usize bytes);
        [[nodiscard]] KITSUNE_API_ static void* Allocate(Usize bytes, Usize alignment);

        KITSUNE_API_ static void Free(void* ptr);

    public:
        [[nodiscard]]
        KITSUNE_API_ static Usize GetDefaultAlignment();

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
        static IMemoryApi* s_MemoryApi;
        static bool s_Initialized;
    };
}
