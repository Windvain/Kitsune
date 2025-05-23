#include "RenderingCore/IGraphicsInstance.h"

#if defined(KITSUNE_INCLUDE_D3D12_IMPLEMENTATION)
    #define KITSUNE_NO_INCLUDE_D3DX12_H_
    #include "RenderingCore/D3D12/D3D12Instance.h"
#endif

namespace Kitsune
{
    SharedPtr<IGraphicsInstance> CreateGraphicsInstance(GraphicsBackend backend)
    {
        switch (backend)
        {
        case GraphicsBackend::DirectX12:
        {
#if !defined(KITSUNE_BUILD_RELEASE)
            return MakeShared<D3D12Instance>(true);
#else
            return MakeShared<D3D12Instance>(false);
#endif
        }
        default:
            KITSUNE_UNREACHABLE();
        }
    }
}
