#include "Foundation/Memory/Memory.h"

#include "Foundation/Memory/CMallocApi.h"
#include "Foundation/Memory/BadAllocException.h"

namespace Kitsune
{
    IMemoryApi* Memory::s_MemoryApi = nullptr;
    bool Memory::s_Initialized = false;

    void Memory::InitializeExplicit()
    {
        if (s_Initialized) return;          // Might be called more than once..

        s_MemoryApi = new (std::nothrow) CMallocApi();
        if (s_MemoryApi == nullptr) throw BadAllocException();

        s_Initialized = true;
    }

    void Memory::Shutdown()
    {
        delete s_MemoryApi;
        s_Initialized = false;
    }

    void* Memory::TryAllocate(Usize bytes)
    {
        if (!s_Initialized) Memory::InitializeExplicit();
        return s_MemoryApi->TryAllocate(bytes, s_MemoryApi->GetDefaultAlignment());
    }

    void* Memory::TryAllocate(Usize bytes, Usize alignment)
    {
        if (!s_Initialized) Memory::InitializeExplicit();
        return s_MemoryApi->TryAllocate(bytes, alignment);
    }

    void* Memory::Allocate(Usize bytes)
    {
        void* ptr = TryAllocate(bytes);
        if (ptr == nullptr)
            s_MemoryApi->OutOfMemory(bytes, s_MemoryApi->GetDefaultAlignment());

        return ptr;
    }

    void* Memory::Allocate(Usize bytes, Usize alignment)
    {
        void* ptr = TryAllocate(bytes, alignment);
        if (ptr == nullptr) s_MemoryApi->OutOfMemory(bytes, alignment);

        return ptr;
    }

    void Memory::Free(void* ptr)
    {
        if (!s_Initialized) Memory::InitializeExplicit();
        s_MemoryApi->Free(ptr);
    }
}
