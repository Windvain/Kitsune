kitsune_add_library(
    TARGET KitsuneRenderingCore
    SOURCES
    "GraphicsBackend.h"
    "ICommandQueue.h"
    "IGraphicsDevice.cpp"
    "IGraphicsDevice.h"
    "IPhysicalDevice.h"
    "IRenderTarget.h"
    "ISwapChain.h"
    "ITexture.h"
    "RenderException.h"

    DEPENDENCIES
    KitsuneFoundation
)

option(KITSUNE_INCLUDE_D3D12_IMPLEMENTATION "Compile Kitsune with D3D12 support." ON)

if (KITSUNE_INCLUDE_D3D12_IMPLEMENTATION AND MSVC)
    kitsune_add_library(
        TARGET KitsuneRenderingCore_D3D12
        SOURCES
        "D3D12/D3D12CommandQueue.cpp"
        "D3D12/D3D12CommandQueue.h"
        "D3D12/D3D12DescriptorHeap.cpp"
        "D3D12/D3D12DescriptorHeap.h"
        "D3D12/D3D12GraphicsDevice.cpp"
        "D3D12/D3D12GraphicsDevice.h"
        "D3D12/D3D12PhysicalDevice.cpp"
        "D3D12/D3D12PhysicalDevice.h"
        "D3D12/D3D12RenderTarget.cpp"
        "D3D12/D3D12RenderTarget.h"
        "D3D12/D3D12SwapChain.cpp"
        "D3D12/D3D12SwapChain.h"
        "D3D12/D3D12Texture.cpp"
        "D3D12/D3D12Texture.h"
        "D3D12/DirectX12Core.h"

        DEPENDENCIES
        KitsuneFoundation
        KitsuneApplicationCore

        "d3d12.lib"
        "dxgi.lib"
    )

    # HACK: We include <d3dx12.h> in some header files.
    target_link_libraries(KitsuneRenderingCore_D3D12 PUBLIC Microsoft::DirectX-Headers)

    target_link_libraries(KitsuneRenderingCore PUBLIC KitsuneRenderingCore_D3D12)
    target_compile_definitions(KitsuneRenderingCore PRIVATE KITSUNE_INCLUDE_D3D12_IMPLEMENTATION)
endif()
