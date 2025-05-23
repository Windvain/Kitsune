#include "RenderingCore/D3D12/D3D12Fence.h"

namespace Kitsune
{
    D3D12Fence::D3D12Fence(const DirectX::ComPtr<ID3D12Device2>& device, Uint64 value)
        : m_Value(value)
    {
        DirectX::ThrowIfFailed(device->CreateFence(value, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)),
                               "Failed to create a DirectX 12 fence.");

        m_Event = ::CreateEventW(nullptr, false, false, nullptr);
        if (m_Event == NULL)
            throw RenderException("Failed to create a Windows event object.");
    }

    D3D12Fence::~D3D12Fence()
    {
        ::CloseHandle(m_Event);
    }

    void D3D12Fence::Signal()
    {
        ++m_Value;
    }

    void D3D12Fence::Wait()
    {
        if (m_Fence->GetCompletedValue() < m_Value)
        {
            if (FAILED(m_Fence->SetEventOnCompletion(m_Value, m_Event)))
                throw RenderException("Failed to set fence on-completion event.");

            ::WaitForSingleObject(m_Event, INFINITE);
        }
    }
}
