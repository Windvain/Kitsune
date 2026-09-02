#include "Foundation/Memory/Memory.h"
#include "Foundation/Memory/CMallocAPI.h"

namespace Kitsune
{
    bool Memory::s_Initialized = false;
    MemoryAPI* Memory::s_MemoryAPI = nullptr;

    bool Memory::InitializeExplicit()
    {
        // The memory subsystem is lazily initialized, so there might be
        // multiple calls to InitializeExplicit().
        if (s_Initialized)
            return true;

        s_MemoryAPI = new (std::nothrow) CMallocAPI();
        if (s_MemoryAPI != nullptr)
            s_Initialized = true;

        return s_Initialized;
    }

    void Memory::Shutdown()
    {
        delete s_MemoryAPI;
        s_Initialized = false;
    }

    void* Memory::TryAllocate(Usize bytes)
    {
        if ((bytes == 0) || !Memory::InitializeExplicit())
            return nullptr;

        return s_MemoryAPI->TryAllocate(bytes);
    }

    void* Memory::TryAllocate(Usize bytes, Usize alignment)
    {
        if ((bytes == 0) || ((alignment & (alignment - 1)) != 0) ||
            !Memory::InitializeExplicit())
        {
            return nullptr;
        }

        return s_MemoryAPI->TryAllocate(bytes, alignment);
    }

    void* Memory::Allocate(Usize bytes)
    {
        void* pointer = TryAllocate(bytes);
        if (pointer == nullptr)
            throw BadAllocException();

        return pointer;
    }

    void* Memory::Allocate(Usize bytes, Usize alignment)
    {
        void* pointer = TryAllocate(bytes, alignment);
        if (pointer == nullptr)
            throw BadAllocException();

        return pointer;
    }

    void Memory::Free(void* pointer, Usize bytes)
    {
        if (pointer == nullptr)
            return;

        // No need to lazily initialize. If the user called Free() without calling
        // Allocate(), that means that the pointer is invalid anyways.
        s_MemoryAPI->Free(pointer, bytes);
    }
}
