#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Utilities/NonCopyable.h"

namespace Kitsune
{
    // An abstract class to a memory API, for example to the C runtime library, or
    // to 3rd party libraries, like jemalloc or mimalloc.
    class MemoryAPI : public NonCopyable
    {
    public:
        virtual ~MemoryAPI() = default;

        [[nodiscard]] virtual void* TryAllocate(Usize bytes) = 0;
        [[nodiscard]] virtual void* TryAllocate(Usize bytes, Usize alignment) = 0;

        virtual void Free(void* pointer, Usize bytes) = 0;

    public:
        [[nodiscard]]
        virtual Usize GetDefaultAlignment() const = 0;

    public:
        // These functions are used for implementing the new/delete operators. MemoryAPI
        // doesn't force the implementation to keep the size of the block, which is a
        // problem when we're trying to free the memory used for polymorphic types.
        [[nodiscard]] virtual void* TryNew(Usize bytes, Usize alignment) = 0;
        virtual void Delete(void* pointer) = 0;
    };
}
