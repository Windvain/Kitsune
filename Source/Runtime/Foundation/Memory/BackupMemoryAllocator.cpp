#include "Foundation/Memory/BackupMemoryAllocator.h"
#include "Launch/DefaultEngineLoop.h"

#include "Foundation/Memory/BadAllocException.h"
#include "Foundation/Diagnostics/InvalidArgumentException.h"

namespace Kitsune
{
    void* BackupMemoryAllocator::Allocate(Usize size, Usize alignment)
    {
        if (DefaultEngineLoop::GetInstance() == nullptr)
            return nullptr;

        alignment = KITSUNE_MAX(alignment, __STDCPP_DEFAULT_NEW_ALIGNMENT__);

        if ((alignment % 2) != 0)
            throw InvalidArgumentException("Alignment must be a power of two.");

        auto& pool = DefaultEngineLoop::GetInstance()->GetBackupMemoryPool();
        Usize offset = sizeof(void*) + (alignment - 1);

        void* ptr = pool.TryAllocate(size + offset);
        if (ptr == nullptr)
            throw BadAllocException();

        void** returnedPtr = reinterpret_cast<void**>(
            (reinterpret_cast<Uintptr>(ptr) + offset) & ~(alignment - 1));

        returnedPtr[-1] = ptr;
        return returnedPtr;
    }

    void* BackupMemoryAllocator::Allocate(Usize size)
    {
        return Allocate(size, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
    }

    void BackupMemoryAllocator::Free(void* ptr)
    {
        if ((ptr == nullptr) || (DefaultEngineLoop::GetInstance() == nullptr))
            return;

        auto& pool = DefaultEngineLoop::GetInstance()->GetBackupMemoryPool();
        void* original = reinterpret_cast<void**>(ptr)[-1];

        pool.Free(original);
    }
}
