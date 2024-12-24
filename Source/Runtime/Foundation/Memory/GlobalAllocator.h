#pragma once

#include "Foundation/Memory/Memory.h"

namespace Kitsune
{
    class GlobalAllocator
    {
    public:
        GlobalAllocator() = default;

        GlobalAllocator(const GlobalAllocator&) = default;
        GlobalAllocator(GlobalAllocator&&) = default;
        ~GlobalAllocator() = default;

    public:
        GlobalAllocator& operator=(const GlobalAllocator&) = default;
        GlobalAllocator& operator=(GlobalAllocator&&) = default;

    public:
        void* Allocate(Usize bytes)
        {
            return Memory::Allocate(bytes);
        }

        void* Allocate(Usize bytes, Usize align)
        {
            return Memory::Allocate(bytes, align);
        }

        void Free(void* ptr)
        {
            Memory::Free(ptr);
        }
    };
}
