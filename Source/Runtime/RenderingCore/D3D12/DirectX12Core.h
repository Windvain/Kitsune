#pragma once

#include "Foundation/Common/Macros.h"
#include "Foundation/Common/Predefined.h"

#include "RenderingCore/RenderException.h"

#include <directx/d3dx12.h>
#include <directx/d3d12.h>

#include <dxgi1_6.h>
#include <wrl.h>

// ld.exe requires dxguids.h to be included, or else it will complain with:
// undefined reference to `_GUID const& __mingw_uuidof<DIRECTX_INTERFACE>()'
#if !defined(KITSUNE_COMPILER_MSVC)
    #include <dxguids/dxguids.h>
#endif

namespace Kitsune::DirectX
{
    using ::Microsoft::WRL::ComPtr;

    KITSUNE_FORCEINLINE void ThrowIfFailed(HRESULT result, const char* message)
    {
        if (FAILED(result))
            throw RenderException(message);
    }
}
