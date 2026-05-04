#pragma once

#include "Foundation/Common/Types.h"
#include "Foundation/Utilities/EnumFlags.h"

namespace Kitsune
{
    // Specifies the usage of the buffer.
    enum class GpuBufferUsage
    {
        None = 0,
        VertexBuffer = 1 << 0,
        IndexBuffer = 1 << 1,
        TransferSource = 1 << 2,
        TransferDestination = 1 << 3,
    };

    KITSUNE_OVERLOAD_FLAGS_OPERATORS(GpuBufferUsage);

    // Contains specifications for buffer creation.
    struct GpuBufferSpecifications
    {
        Uint64 Size;
        GpuBufferUsage Usage;
    };

    // Linear array of data on the GPU.
    class GpuBuffer
    {
    public:
        virtual ~GpuBuffer() = default;

    public:
        [[nodiscard]] virtual void* Map() = 0;
        virtual void Unmap() = 0;
    };
}
