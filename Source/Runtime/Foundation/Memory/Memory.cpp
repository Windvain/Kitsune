#include "Foundation/Memory/Memory.h"

#include "Foundation/Memory/CMallocApi.h"
#include "Foundation/Memory/BadAllocException.h"

namespace Kitsune
{
    IMemoryApi* Memory::s_MemoryApi = nullptr;
    bool Memory::s_Initialized = false;

    bool Memory::InitializeExplicit()
    {
        // The memory subsystem is lazily initialized, so there might be
        // multiple calls to InitializeExplicit().
        if (s_Initialized)
            return;

        s_MemoryApi = new (std::nothrow) CMallocApi();
        if (s_MemoryApi != nullptr)
            s_Initialized = true;

        return s_Initialized;
    }

    void Memory::Shutdown()
    {
        delete s_MemoryApi;
        s_Initialized = false;
    }

    void* Memory::TryAllocate(Usize bytes)
    {
        if (bytes == 0)
            return nullptr;

        if (!s_Initialized)
            Memory::InitializeExplicit();

        return s_MemoryApi->TryAllocate(bytes, s_MemoryApi->GetDefaultAlignment());
    }

    void* Memory::TryAllocate(Usize bytes, Usize alignment)
    {
        if (bytes == 0)
            return nullptr;

        if (!s_Initialized)
            Memory::InitializeExplicit();

        return s_MemoryApi->TryAllocate(bytes, alignment);
    }

    void* Memory::Allocate(Usize bytes)
    {
        void* ptr = TryAllocate(bytes);
        if (ptr == nullptr)
            throw BadAllocException();

        return ptr;
    }

    void* Memory::Allocate(Usize bytes, Usize alignment)
    {
        void* ptr = TryAllocate(bytes, alignment);
        if (ptr == nullptr)
            throw BadAllocException();

        return ptr;
    }

    void Memory::Free(void* ptr)
    {
        if (ptr == nullptr)
            return;

        // No need to lazily initialize. If they called Free() without calling Allocate(),
        // that means that the pointer is invalid anyways.
        s_MemoryApi->Free(ptr);
    }
}
