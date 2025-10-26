#include "Foundation/Memory/Memory.h"

#include <Windows.h>
#include "Foundation/Memory/BadAllocException.h"

namespace Kitsune
{
    void* Memory::VirtualAllocate(Usize bytes, MemoryProtection protection)
    {
        DWORD flProt = (protection == MemoryProtection::ReadWrite) ? PAGE_READWRITE : PAGE_EXECUTE_READWRITE;
        void* ptr = ::VirtualAlloc(nullptr, bytes, MEM_COMMIT | MEM_RESERVE, flProt);

        if (ptr == nullptr)
            throw BadAllocException();

        return ptr;
    }

    void Memory::VirtualFree(void *ptr, Usize /* bytes */)
    {
        ::VirtualFree(ptr, 0, MEM_RELEASE);
    }
}
