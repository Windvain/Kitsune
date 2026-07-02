#pragma once

#include <new>              // IWYU pragma: keep
#include <type_traits>

#include "Foundation/Common/Types.h"
#include "Foundation/Templates/Forward.h"

#include "Foundation/Memory/MemoryApi.h"
#include "Foundation/Memory/BadAllocException.h"

namespace Kitsune
{
    // Specifies the protection placed on a page of memory.
    enum class MemoryProtection
    {
        ReadWrite,
        ReadWriteExecute
    };

    // The memory class. Contains most of the common functions for dealing with
    // memory.
    class KITSUNE_API Memory
    {
    public:
        static bool InitializeExplicit();
        static void Shutdown();

    public:
        [[nodiscard]] static void* TryAllocate(Usize bytes);
        [[nodiscard]] static void* TryAllocate(Usize bytes, Usize alignment);

        [[nodiscard]] static void* Allocate(Usize bytes);
        [[nodiscard]] static void* Allocate(Usize bytes, Usize alignment);

        static void Free(void* pointer, Usize bytes);

    public:
        [[nodiscard]]
        static void* VirtualAllocate(Usize bytes, MemoryProtection protection);

        static void VirtualFree(void* pointer, Usize bytes);

    public:
        template<typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        inline static T* ConstructAt(void* pointer, Args&&... args)
        {
            return new (pointer) T(Forward<Args>(args)...);
        }

        template<typename T>
        inline static void DestroyAt(T* pointer)
        {
            pointer->~T();
        }

        template<typename T, typename... Args>
            requires std::is_constructible_v<T, Args...>
        [[nodiscard]]
        inline static T* New(Args&&... args)
        {
            void* pointer = s_MemoryApi->TryNew(sizeof(T), alignof(T));
            if (pointer == nullptr)
                throw BadAllocException();

            return ConstructAt<T>(pointer, Forward<Args>(args)...);
        }

        template<typename T>
        inline static void Delete(T* pointer)
        {
            DestroyAt(pointer);
            s_MemoryApi->Delete(pointer);
        }

    public:
        Memory() = delete;
        ~Memory() = delete;

    private:
        static bool s_Initialized;
        static MemoryApi* s_MemoryApi;
    };
}
