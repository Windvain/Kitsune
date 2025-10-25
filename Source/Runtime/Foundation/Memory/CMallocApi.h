#pragma once

#include <cstdlib>
#include "Foundation/Memory/IMemoryApi.h"

namespace Kitsune
{
    class CMallocApi : public IMemoryApi
    {
    public:
        void* TryAllocate(Usize bytes, Usize alignment) override;
        void Free(void* ptr) override;

        inline Usize GetDefaultAlignment() const override { return s_DefaultAlignment; }

    private:
        static constexpr Usize s_DefaultAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
    };
}
