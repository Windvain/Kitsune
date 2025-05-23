#pragma once

#include <directx/d3dx12.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>

#include "Foundation/Common/Macros.h"
#include "RenderingCore/RenderException.h"

namespace Kitsune::DirectX
{
    using ::Microsoft::WRL::ComPtr;

    KITSUNE_FORCEINLINE void ThrowIfFailed(HRESULT result, const char* message)
    {
        if (FAILED(result))
            throw RenderException(message);
    }
}
