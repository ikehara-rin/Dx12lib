#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class SpriteRenderer
{
public:
    SpriteRenderer();

    void Initialize(ID3D12Device* device, ID3D12DescriptorHeap* srvHeap, UINT descriptorSize);
    void DrawCell(ID3D12GraphicsCommandList* commandList, ID3D12Resource* texture, float x, float y, float cellSize);

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};

    Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
    uint8_t* m_pConstantBufferWO = nullptr;

    D3D12_GPU_DESCRIPTOR_HANDLE m_srvHandleGPU{};
};
