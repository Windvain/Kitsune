#include "Foundation/Memory/Memory.h"

#include <Windows.h>
#include "Foundation/Memory/BadAllocException.h"

namespace Kitsune
{
    void* Memory::VirtualAllocate(Usize bytes, MemoryProtection protection)
    {
        DWORD pageProtection = PAGE_EXECUTE_READWRITE;
        if (protection == MemoryProtection::ReadWrite)
            pageProtection = PAGE_READWRITE;

        void* pointer = ::VirtualAlloc(nullptr, bytes, MEM_COMMIT | MEM_RESERVE,
                                       pageProtection);

        if (pointer == nullptr)
            throw BadAllocException();

        return pointer;
    }

    void Memory::VirtualFree(void* pointer, Usize /* bytes */)
    {
        ::VirtualFree(pointer, 0, MEM_RELEASE);
    }
}
