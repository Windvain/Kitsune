#include "RenderingCore/IGraphicsDevice.h"
#include "RenderingCore/RenderException.h"

#if defined(KITSUNE_INCLUDE_D3D12_IMPLEMENTATION)
    #include "RenderingCore/D3D12/D3D12GraphicsDevice.h"
#endif

namespace Kitsune
{
    SharedPtr<IGraphicsDevice> CreateGraphicsDevice(const GraphicsDeviceSpecs& specs)
    {
        switch (specs.Backend)
        {
#if defined(KITSUNE_INCLUDE_D3D12_IMPLEMENTATION)
        case GraphicsBackend::DirectX12:
            return MakeShared<D3D12GraphicsDevice>(specs);
#endif

        case GraphicsBackend::None:
        default:
            throw RenderException("Unknown graphics backend.");
        }
    }
}
